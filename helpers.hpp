#pragma once
#include <bitset>
#include <vector>
#include <iostream>
#include <sstream>
#include "api.hpp"

namespace utils
{

    inline std::string cout_vec_uint8_hex(const std::vector<uint8_t>& v_u8)
    {
        std::stringstream ss;
        for(const auto& hex : v_u8)
            ss << "0x" << std::hex << static_cast<int>(hex) << " ";
        return ss.str();
    }

    template<typename T>
    inline std::string int_to_hex(T i)
    {
        std::stringstream ss;
        ss << "0x" << std::hex << i;
        return ss.str();
    }

    inline std::string descr_stringify(api::source src)
    {
        switch (src)
        {
            case api::VMC: return "VMC";
            case api::POS: return "POS";
            default:
                return "unknown";
        }
    }
}
