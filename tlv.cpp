#include "tlv.hpp"
#include <cstdio>
#include <iostream>
#include <bitset>
#include <utility>
#include <functional>
#include "helpers.hpp"

namespace app {

    #define SAFE_READ(data, byte_size, size, file_size, file)      \
        fpos_t pos;                                                \
        fgetpos(file, &pos);                                       \
        if((file_size - pos.__pos) < size)                         \
            throw std::runtime_error("Not enought data for read"); \
        if(!std::fread(data, byte_size, size, file))               \
            throw std::runtime_error("Fail read file")
        
    bool is_eof(FILE* file)
    {
        int ch = getc(file);
        if(feof(file)) return true;
        if(ch == 0)
        {
            getc(file);
            if(feof(file)) return true;
            fseek(file, -2, SEEK_CUR);
            return false;
        } 
        fseek(file, -1, SEEK_CUR);
        return false;
    }

    tlv_parser::tlv_parser(FILE* file) 
    : 
    _file(file)
    {
        fseek(file, 0L, SEEK_END);
        _file_sz = ftell(file);
        rewind(file);
    }

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
        SAFE_READ(&d1, sizeof(uint8_t), 1, _file_sz, _file);
        return d1;
    }

    void tlv_parser::parse_nested(std::vector<uint8_t>& tag, std::vector<tlv>& tlvs)
    {
        tlv t;
        t.tag = std::move(tag);
        t.len = read_length();

        cout_uint8_hex(t.tag[0]);
        if(t.tag.size() == 2)
            cout_uint8_hex(t.tag[1]);
        cout_vec_uint8_hex(t.len.buff);

        if(t.is_nested())
            t.value = std::move(read_value(t.len.size, true));
        else
            t.value = std::move(read_value(t.len.size));

        if (!IS_PRIMITIVE_TAG(t.tag[t.tag.size()-1]))
        {
            tlvs.emplace_back(std::move(t));
            tag = std::move(read_tag());
            parse_nested(tag, tlvs);
        }
        else
        {
            tlvs.emplace_back(std::move(t));
            if(is_eof(_file)) return;
            tag = std::move(read_tag());
            parse_nested(tag, tlvs);
        }
    }

    tlv tlv_parser::parse_primitive(const std::vector<uint8_t>& tag)
    {
        tlv t;
        t.tag = std::move(tag);
        t.len = read_length();
        t.value = read_value(t.len.size);
        return t;
    }

    std::vector<tlv> tlv_parser::parse()
    {
        std::vector<tlv> tlvs;
        std::vector<uint8_t> tag = read_tag();

        if (IS_PRIMITIVE_TAG(tag[0]))
        {
            tlvs.emplace_back(std::move(parse_primitive(tag)));
            return tlvs;
        }

        parse_nested(tag, tlvs);
        return tlvs;
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

        uint32_t sz = 0;
        if(len.size == 1)
            sz =    len.buff[0] & 0xFF;
        else if(len.size == 2)
            sz =    ((len.buff[0] & 0xFF) << 8) | 
                    (len.buff[1] & 0xFF);
        else if(len.size == 3)
            sz =    ((len.buff[0] & 0xFF) << 16) |
                    ((len.buff[1] & 0xFF) << 8) | 
                    (len.buff[2] & 0xFF);
        else if(len.size == 4)
            sz =    ((len.buff[0] & 0xFF) << 24) |
                    ((len.buff[1] & 0xFF) << 16) |
                    ((len.buff[2] & 0xFF) << 8) |
                    (len.buff[3] & 0xFF);
        else
            throw std::runtime_error("Invalid length > 4 bytes");
        len.size = sz;

        return len;
    }

    std::vector<uint8_t> tlv_parser::read_value(const uint32_t size, bool is_no_offset)
    {
        std::vector<uint8_t> value;
        value.resize(size);

        fpos_t start_rpos;
        fgetpos(_file, &start_rpos);  

        //Do safe for read out of range
        SAFE_READ(&value[0], sizeof(uint8_t), size, _file_sz, _file);

        if(is_no_offset)
            fseek(_file , start_rpos.__pos, SEEK_SET);

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

    void tlv::info()
    {
        std::cout << "tlv\n{\n";
        std::cout << "\ttag: 0x" << std::hex << get_tag_number() << std::endl;
        std::cout.unsetf(std::ios::hex);
        std::cout << "\tlength: " << len.size << std::endl;
        std::cout << "\tvalue: ";
        cout_vec_uint8_hex(value);
        std::cout << "\n}\n";
    }
}