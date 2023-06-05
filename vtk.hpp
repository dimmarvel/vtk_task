#pragma once
#include "tlv.hpp"
#include <string>
#include <vector>

namespace app
{
    constexpr const uint16_t APP_MAX_MSG_SIZE = 65533;

    struct vtk
    {
        uint16_t    len;
        uint16_t    descrim;
        std::vector<tlv> tlvs{};

        std::string info();
        
    private:
        std::string tlvs_to_string();
    };
}