#include "parser.hpp"
#include <stdexcept>

namespace app::parser
{
    #define SAFE_READ(data, byte_size, size, file_size, file)       \
        fpos_t pos;                                                 \
        fgetpos(file, &pos);                                        \
        if((file_size - pos.__pos) < (size * byte_size))            \
            throw std::runtime_error(                               \
                "Not enought data for read(" +                      \
                std::to_string((file_size - pos.__pos)) +           \
                " < " + std::to_string(size * byte_size) + ")");    \
        if(!std::fread(data, byte_size, size, file))                \
            throw std::runtime_error("Fail read file")

    tlv_parser::tlv_parser(const std::string& path)
    {
        _file = fopen(path.c_str(), "rb"); // Check if cant open then throw?
        fseek(_file, 0L, SEEK_END);
        _file_sz = ftell(_file);
        rewind(_file);
    }

    tlv_parser::~tlv_parser()
    {
        if(_file != nullptr)
            fclose(_file);
    }

    uint8_t tlv_parser::read_byte()
    {
        uint8_t u8;
        SAFE_READ(&u8, sizeof(uint8_t), 1, _file_sz, _file);
        return u8;
    }

    void tlv_parser::parse_nested(std::vector<uint8_t>& tag, std::vector<tlv>& tlvs)
    {
        tlv t;
        t.tag = std::move(tag);
        t.len = read_length();

        if(t.is_nested())
            t.value = std::move(read_value(t.len.size, true));
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

    std::vector<tlv> tlv_parser::parse()
    {
        std::vector<tlv> tlvs;
        std::vector<uint8_t> tag = read_tag();

        if (tlv::is_primitive_tag(tag[0]))
        {
            tlvs.emplace_back(std::move(parse_primitive(tag)));
            while(!is_eof())
            {
                tag = read_tag();
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
        else
            throw std::runtime_error("Invalid length > 3 bytes");
        len.size = sz;

        return len;
    }

    std::vector<uint8_t> tlv_parser::read_value(const uint32_t size, bool is_no_offset)
    {
        std::vector<uint8_t> value;
        value.resize(size);

        fpos_t start_rpos;
        fgetpos(_file, &start_rpos);  

        SAFE_READ(&value[0], sizeof(uint8_t), size, _file_sz, _file);
        if(is_no_offset)
            fseek(_file , start_rpos.__pos, SEEK_SET);

        return value;
    }

    bool tlv_parser::is_eof()
    {
        int c = getc(_file);
        if(feof(_file)) return true;

        if(c == 0)
        {
            getc(_file);
            if(feof(_file)) return true;

            fseek(_file, -2, SEEK_CUR);
            return false;
        } 

        fseek(_file, -1, SEEK_CUR);
        return false;
    }
}