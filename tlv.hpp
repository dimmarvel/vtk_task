#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>

constexpr std::size_t MAX_LNGTH = 65535;

namespace app
{
    #define IS_PRIMITIVE_TAG(tag) ((tag & 0x20) != 0x20)

    struct length
    {
        std::vector<uint8_t> buff;
        uint32_t size;
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
        bool is_nested() { return (tag[0]) & (1<<(5)); }
        bool is_primitive_tag() { return ((tag[0] & 0x20) != 0x20); }
        void info();
    };

    class tlv_parser
    {
    public:
        tlv_parser(FILE* file);
        ~tlv_parser();

    public:
        std::vector<tlv> parse();
        std::vector<uint8_t> read_tag();
        length read_length();
        std::vector<uint8_t> read_value(const uint32_t size, bool is_no_offset = false);

        void parse_nested(std::vector<uint8_t>& tag, std::vector<tlv>& tlvs);
        tlv parse_primitive(const std::vector<uint8_t>& tag);
        
    private:
        uint8_t read_byte();
        //Return incomplite length, only first byte
        length read_length_size();

    private:
        FILE* _file;
        size_t _file_sz;
    };

}