#pragma once
#include "tlv.hpp"
#include <string>
#include <vector>

namespace app::parser
{
    class tlv_parser
    {
    public:
        tlv_parser() = default;

        tlv_parser(const tlv_parser& copy) = delete;
        tlv_parser& operator=(const tlv_parser& copy) = delete;
        
    public:
        std::vector<tlv> parse(std::vector<uint8_t>&& buff);

    private:
        std::vector<uint8_t> read_tag();
        length read_length();
        std::vector<uint8_t> read_value(const uint32_t size, bool is_offset = true);

        void parse_nested(std::vector<uint8_t>& tag, std::vector<tlv>& tlvs);
        tlv parse_primitive(const std::vector<uint8_t>& tag);
        
        bool is_eof();
        //Return incomplite length, only first byte
        length read_length_size();

    private:
        size_t _offset = 0;
        std::vector<uint8_t> _buff;
    };

}