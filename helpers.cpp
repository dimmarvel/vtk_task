#include "helpers.hpp"
#include "tlv.hpp"
#include <random>
#include <cstring>
#include <bitset>

namespace app
{
    int rnd(int min, int max)
    {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist6(min, max);
        return dist6(rng);
    }

    std::string rand_str()
    {
        int size = rnd(1, 5);
        const int MAX = 26;
        char alpha[MAX] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                            'h', 'i', 'j', 'k', 'l', 'm', 'n',
                            'o', 'p', 'q', 'r', 's', 't', 'u',
                            'v', 'w', 'x', 'y', 'z' };
        std::string result;
        for (int i = 0; i < size; ++i)
            result += alpha[rnd(1, MAX-1)];

        return result;
    }

    std::string rand_tag_bytes()
    {
        std::string result;

        for (int i = 0; i < 8; ++i)
            result += std::to_string(rnd(0, 1));

        if(is_2byte_tag(create_tag(result)))
        {
            for(int i = 8; i < 16; ++i)
                result += std::to_string(rnd(0, 1));
        }

        return result;
    }

    bool is_eof(FILE* file)
    {
        getc(file);
        if(feof(file)) return true;
        fseek(file, -1, SEEK_CUR);
        return false;
    }

    uint16_t create_tag(const std::string& tag)
    {
        return static_cast<uint16_t>(std::bitset<16>(tag).to_ulong());
    }

    tlv create_rnd_tlv()
    {
        std::string value = rand_str();
        std::string bytes = rand_tag_bytes();
        tlv packet{create_tag(bytes), static_cast<uint16_t>(value.length() + 1), value.data()};
        return packet;
    }
}