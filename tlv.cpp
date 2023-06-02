#include "tlv.hpp"
#include <cstdio>
#include <iostream>
#include <bitset>
#include <utility>
#include <functional>

namespace app {
        
    bool is_eof(FILE* file)
    {
        getc(file);
        if(feof(file)) return true;
        fseek(file, -1, SEEK_CUR);
        return false;
    }

    tlv_parser::tlv_parser(FILE* file) : _file(file)
    {}

    tlv_parser::~tlv_parser()
    {}

    tlv::~tlv()
    {
        tag.clear();
        value.clear();
        len.buff.clear();
    }

    uint8_t tlv_parser::read_byte()
    {
        uint8_t d1;
        SAFE_READ(&d1, sizeof(uint8_t), 1, _file);
        return d1;
    }

    std::vector<tlv> tlv_parser::parse_nested(const std::vector<uint8_t>& tag)
    {
        //Doesnt mplement
        std::vector<tlv> f; 
        return f;
    }

    std::vector<tlv> tlv_parser::parse_primitive(const std::vector<uint8_t>& tag)
    {
        std::vector<tlv> tlvs;
        tlv t;
        t.tag = std::move(tag);
        t.len = read_length();
        t.value = read_value(t.len.size);
        tlvs.emplace_back(t);

        while(!is_eof(_file))
        {
            t.tag = read_tag();
            t.len = read_length();
            t.value = read_value(t.len.size);
            tlvs.emplace_back(t);
        }
        
        return tlvs;
    }

    std::vector<tlv> tlv_parser::parse()
    {
        std::vector<uint8_t> tag = read_tag();

        if ((tag[0] & 0x20) != 0x20)
            return parse_primitive(tag);
        return parse_nested(tag);
    }

    std::vector<uint8_t> tlv_parser::read_tag()
    {
        std::vector<uint8_t> tag; 
        tag.push_back(read_byte());
        if ((tag[tag.size()-1] & 0x1F) == 0x1F) 
        {
            tag.push_back(read_byte());
            while (((tag[tag.size()-1] >> 7) & 0x01) == 1)
            {
                if(tag.size() == 3)
                    throw std::runtime_error("Error tag parsing more than 3 bytes");
                tag.push_back(read_byte());
            }
        } 
        return tag;
    }

    length tlv_parser::read_length_size()
    {
        length len;
        len.buff.push_back(read_byte());
        len.size = 1;

        if (((len.buff[0] >> 7) & 0x01) == 0x01)
        {
            if (len.buff.size()-1 == 3)
                throw std::runtime_error("Error length parsing more than 3 bytes");
            len.size = (len.buff[0] & 0x7F);
        }

        return len;
    }

    length tlv_parser::read_length()
    {
        length len = std::move(read_length_size());

        if(len.buff.empty())
            throw std::runtime_error("Length is empty");

        if (len.buff[0] == 0x80)
            throw std::runtime_error("Invalid length = 0x80");

        if (len.buff[0] < 0x80)
        {
            len.size = len.buff[0] & 0x7F;
            return len;
        }

        for(uint16_t i = 0; i < len.size; ++i)
            len.buff.push_back(read_byte());

        uint16_t sz = 0;
        if(len.size == 1)
            sz =    len.buff[0] & 0xFF;
        else if(len.size == 2)
            sz =    ((len.buff[1] & 0xFF) << 8) | 
                    (len.buff[2] & 0xFF);
        else if(len.size == 3)
            sz =    ((len.buff[1] & 0xFF) << 8) | 
                    (len.buff[2] & 0xFF);
        else
            throw std::runtime_error("Invalid length > 3 bytes");
        len.size = sz;

        return len;
    }

    std::vector<uint8_t> tlv_parser::read_value(const uint16_t size)
    {
        std::vector<uint8_t> value;
        value.resize(size);
        //Do safe for read out of range
        SAFE_READ(&value[0], sizeof(uint8_t), size, _file);
        return value;
    }

    uint32_t tlv::get_tag_number()
    {
        if(tag.empty())
            throw std::runtime_error("Tag is empty");

        uint32_t tag_number = tag[0];
        if((tag[0] & 0x1F) == 0x1F && tag.size() > 1)
        {
            tag_number = (tag_number << 8) | tag[1];
            if(tag.size() == 3)
                tag_number = (tag_number << 8) | tag[2];
        }
        
        return tag_number;
    }

}