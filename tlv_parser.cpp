#include "tlv_parser.hpp"
#include <stdexcept>
#include <algorithm>
#include "helpers.hpp"

namespace app::parser
{
    void tlv_parser::parse_nested(std::vector<uint8_t>& tag, std::vector<tlv>& tlvs)
    {
        tlv t;
        t.tag = std::move(tag);
        t.len = read_length();

        if(t.is_nested())
            t.value = std::move(read_value(t.len.size, false));
        else
            t.value = std::move(read_value(t.len.size));

        if (!tlv::is_primitive_tag(t.tag[0]))
        {
            tlvs.emplace_back(std::move(t));
            tag = std::move(read_tag());
            parse_nested(tag, tlvs);
        }
        else
        {
            tlvs.emplace_back(std::move(t));
            if(is_eof()) return;
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

    std::vector<tlv> tlv_parser::parse(std::vector<uint8_t>&& buff)
    {
        _buff = std::move(buff);
        std::vector<tlv> tlvs;
        std::vector<uint8_t> tag = std::move(read_tag());

        if (tlv::is_primitive_tag(tag[0]))
        {
            tlvs.emplace_back(std::move(parse_primitive(tag)));
            while(!is_eof())
            {
                tag = std::move(read_tag());
                tlvs.emplace_back(std::move(parse_primitive(tag)));
            }
            return tlvs;
        }
        
        parse_nested(tag, tlvs);
        return tlvs;
    }

    std::vector<uint8_t> tlv_parser::read_tag()
    {
        std::vector<uint8_t> tag; 
        tag.push_back(_buff[_offset++]);

        if ((tag[tag.size()-1] & 0x1F) == 0x1F) 
        {
            tag.push_back(_buff[_offset++]);
            while (((tag[tag.size()-1] >> 7) & 0x01) == 1)
            {
                if(tag.size() == 3)
                    throw std::runtime_error("Error tag parsing more than 3 bytes");
                tag.push_back(_buff[_offset++]);
            }
        } 
        return tag;
    }

    length tlv_parser::read_length_size()
    {
        length len;
        len.buff.push_back(_buff[_offset++]);
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
            len.buff.push_back(_buff[_offset++]);

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
        else
            throw std::runtime_error("Invalid length > 3 bytes");
        len.size = sz;

        return len;
    }

    std::vector<uint8_t> tlv_parser::read_value(const uint32_t size, bool is_offset)
    {
        std::vector<uint8_t> value;
        value.reserve(size);

        if(size > _buff.size())
            throw std::runtime_error("Not enought data for read(" +                  
                std::to_string(size) + " > " + std::to_string(_buff.size()) + ")"); 
        
        if((_buff.size() - _offset) < size)
            throw std::runtime_error("Not enought data for read(" +                  
                std::to_string(size) + " > " + std::to_string(_buff.size() - _offset) + ")"); 
        
        std::copy(_buff.begin() + _offset, _buff.begin() + _offset + size, std::back_inserter(value));
        if(is_offset)
            _offset += size;

        return value;
    }

    bool tlv_parser::is_eof()
    {
        return _buff.size() == _offset;
    }
}