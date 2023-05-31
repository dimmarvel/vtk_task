#include "tlv.hpp"
#include "helpers.hpp"
#include <bitset>
#include <iostream>

namespace app
{

    tlv::tlv(uint16_t tag, uint16_t length, const std::vector<uint8_t>& value)
    :
    _tag(tag),
    _length(length),
    _value(value)
    {}

    void tlv::write(FILE* file)
    {
        uint8_t* tag1b = ((uint8_t*)&_tag) + 1;
        uint8_t* tag2b = ((uint8_t*)&_tag);

        if(is_2byte_tag(*tag1b))
        {
            std::fwrite(tag1b, sizeof(uint8_t), 1, file);
            std::fwrite(tag2b, sizeof(uint8_t), 1, file);
        }
        else
        {
            if(*tag1b == 0)
                std::fwrite(tag2b, sizeof(uint8_t), 1, file);
            else
                std::fwrite(tag1b, sizeof(uint8_t), 1, file);
        }
        
        std::fwrite(&_length, sizeof(uint16_t), 1, file);
        std::fwrite(_value.data(), sizeof(char), (size_t)_length, file);

    }
    
    void tlv::read(FILE* file)
    {
        uint8_t tag1b;
        std::fread(&tag1b, sizeof(uint8_t), 1, file);
        
        if(is_2byte_tag(tag1b))
        {
            uint8_t tag2b;
            std::fread(&tag2b, sizeof(uint8_t), 1, file);
            _tag = ((uint16_t)tag1b << 8) | tag2b;
        }
        else
        {
            _tag |= tag1b;
        }

        std::fread(&_length, sizeof(uint16_t), 1, file);
        char buff[_length];
        std::fread(buff, sizeof(char), (size_t)_length, file);
        _value.assign(buff, buff + _length);
    }

    std::string tlv::info()
    {
        tag_descr td = tag_info();
        std::string result = 
            "\n\ttlv {" + 
            td.info() + ", " +
            "length {" + std::to_string(_length) + "}, " +
            "value {";

        for(size_t i = 0; i < _value.size(); ++i)
        {
            descr_handler dh = param_to_string(_value.at(i));
            if(!dh.first.empty())
            {
                std::string str = dh.second(*this, i);
                result += dh.first + "{" + str + "}";
            }
            else // Different formats can be considered invalid or further written simply character by character
            {
                result.push_back(_value[i]);
            }
        }
        
        result += "}}";
        
        return result;
    }

    tag_descr tlv::tag_info()
    {
        tag_descr td;
        uint8_t *tag1b = ((uint8_t *)&_tag) + 1;
        uint8_t *tag2b = ((uint8_t *)&_tag);
        if(is_2byte_tag(*tag1b))
        {
            td.number = ((uint16_t)((*tag1b) & ~0xE0) << 8) | (*tag2b); // set 0 first 3 bit and write to one number
            td.type = is_bit(*tag1b, 5);
            td.b2 = get_tag_2b(*tag2b);
        }
        else
        {
            if(*tag1b == 0)
            {
                td.number |= *tag2b;
                td.type = is_bit(*tag2b, 5);
                td.b2 = get_tag_2b(*tag2b);
            }
            else
            {
                td.number |= *tag1b;
                td.type = is_bit(*tag1b, 5);
                td.b2 = get_tag_2b(*tag1b);
            }
        }

        return td;
    }

    tag_2b tlv::get_tag_2b(uint8_t t)
    {
        if((t >> 6)         == UNIVERSAL)   return UNIVERSAL;
        if((APPLIED & t)    == APPLIED)     return APPLIED;
        if((PRIVATE & t)    == PRIVATE)     return PRIVATE;
        if((CONTEXT_SENSITIVE & t) == CONTEXT_SENSITIVE) return CONTEXT_SENSITIVE;
        return UNKNOWN_TAG;
    }

    std::string tag_descr::info()
    {
        return  "tag{class:" + tag_to_string(b2) + 
                ", type:" + std::to_string(type) + 
                ", number:" + std::to_string(number) + "(" + std::bitset<16>(number).to_string() + ")" + 
                "}";
    }

    std::string tag_to_string(tag_2b t)
    {
        switch (t)
        {
            case UNIVERSAL:         return "universal";
            case APPLIED:           return "applied";
            case CONTEXT_SENSITIVE: return "context sensitive";
            case PRIVATE:           return "private";
            default:
                return "unknown";
        }
    }


    std::string tlv::handle_0x01(size_t& i) const
    {
        std::string tmp;
        for(int j = 0; j < 3; ++j) 
            tmp.push_back(_value[++i]);
        return tmp;
    }

    descr_handler param_to_string(uint8_t id)
    {
        auto param = DESCR_PARAMS.find(id);
        return (param != DESCR_PARAMS.end()) ? 
                param->second : 
                std::make_pair<std::string, handler>("", EMPTY_HANDLER);
    }

    bool is_2byte_tag(uint8_t tag)
    {
        uint8_t mask = (1 << 5) - 1;
        return (mask & tag) == mask;
    }

}
