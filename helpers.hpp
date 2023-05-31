#pragma once
#include <string>
#include <stdio.h>
#include <vector>
#include <array>

namespace app
{
    class tlv;

    const std::array<std::string, 12> MSG_NAMES =
    {
        "IDL", "DIS", "STA",
        "VRP", "FIN", "ABR",
        "CDP", "MFR", "PRS",
        "CON", "DAT", "DSC" 
    };

    int rnd(int min, int max);
    std::vector<uint8_t> rand_str();
    std::vector<uint8_t> rand_tlv_msg();
    std::string rand_tag_bytes();
    bool is_eof(FILE* file);
    tlv create_rnd_tlv();
    uint16_t create_tag(const std::string& tag);
}