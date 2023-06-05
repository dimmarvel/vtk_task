#pragma once
#include <string>
#include <vector>

namespace app
{
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
        
        ~tlv();

        uint32_t get_tag_number();
        bool is_nested() { return (tag.at(0)) & (1<<(5)); }
        static bool is_primitive_tag(const uint8_t& t) { return ((t & 0x20) != 0x20); }
        void info();
    };
}