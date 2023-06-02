#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>

constexpr std::size_t MAX_LNGTH = 65535;

namespace app
{
    #define SAFE_READ(data, byte_size, size, file)  \
        if(!std::fread(data, byte_size, size, file)) \
            throw std::runtime_error("Fail read file")

    struct length
    {
        std::vector<uint8_t> buff;
        uint16_t size;
    };
    
    struct tlv
    {
        std::vector<uint8_t>    tag;
        length                  len;
        std::vector<uint8_t>    value;
        
        tlv() = default;
        tlv(const tlv&) = default;
        tlv& operator=(const tlv&) = default;
        ~tlv();

        uint32_t get_tag_number();
    };

    class tlv_parser
    {
    public:
        tlv_parser(FILE* file);
        ~tlv_parser();

    public:
        std::vector<tlv> parse();

        std::vector<uint8_t> read_tag();
        //Complete length with one byte
        length read_length();
        std::vector<uint8_t> read_value(const uint16_t size);

        std::vector<tlv> parse_nested(const std::vector<uint8_t>& tag);
        std::vector<tlv> parse_primitive(const std::vector<uint8_t>& tag);
        
    private:
        uint8_t read_byte();
        //Return incomplite length, only first byte
        length read_length_size();

    private:
        FILE* _file;
    };

}