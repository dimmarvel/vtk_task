#include <bitset>
#include <vector>
#include <iostream>

void cout_uint8_hex(const uint8_t& u8)
{
    std::cout << std::hex << static_cast<int>(u8) << std::endl;
    std::cout.unsetf(std::ios::hex);
}

void cout_uint32_hex(const uint32_t& u32)
{
    std::cout << std::hex << static_cast<int>(u32) << std::endl;
    std::cout.unsetf(std::ios::hex);
}

void cout_vec_uint8_hex(const std::vector<uint8_t>& v_u8)
{
    for(const auto& hex : v_u8)
        std::cout << std::hex << static_cast<int>(hex);
    std::cout.unsetf(std::ios::hex);
    std::cout << std::endl;
}
