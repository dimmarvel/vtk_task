#pragma once
#include <string>
#include <stdio.h>

namespace app
{
    class tlv;

    int rnd(int min, int max);
    std::string rand_str();
    std::string rand_tag_bytes();
    bool is_eof(FILE* file);
    tlv create_rnd_tlv();
    uint16_t create_tag(const std::string& tag);
}