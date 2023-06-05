#include "tlv.hpp"
#include "helpers.hpp"
#include <iostream>

namespace app {
    
    tlv::~tlv()
    {
        tag.clear();
        value.clear();
        len.buff.clear();
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

    std::string tlv::info()
    {
        return std::string("tlv { tag: " + utils::int_to_hex(get_tag_number())
        + ", length: " + std::to_string(len.size) + ", value: "
        + utils::cout_vec_uint8_hex(value)
        + " }");
    }
}