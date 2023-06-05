#include "tlv.hpp"
#include "helpers.hpp"
#include <cstdio>
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

    void tlv::info()
    {
        std::cout << "tlv { tag: 0x" << std::hex << get_tag_number();
        std::cout.unsetf(std::ios::hex);
        std::cout << ", length: " << len.size << ", value: ";
        cout_vec_uint8_hex(value);
        std::cout << " }\n";
    }
}