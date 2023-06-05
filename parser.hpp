#pragma once
#include "tlv.hpp"
#include <string>
#include <vector>

namespace app::parser
{
    class tlv_parser
    {
    public:
        explicit tlv_parser(const std::string& path);
        ~tlv_parser();

    public:
        std::vector<tlv> parse();
        
    private:
        std::vector<uint8_t> read_tag();
        length read_length();
        std::vector<uint8_t> read_value(const uint32_t size, bool is_no_offset = false);

        void parse_nested(std::vector<uint8_t>& tag, std::vector<tlv>& tlvs);
        tlv parse_primitive(const std::vector<uint8_t>& tag);
        
        bool is_eof();
        uint8_t read_byte();
        //Return incomplite length, only first byte
        length read_length_size();

    private:
        FILE* _file;
        size_t _file_sz;
    };

}