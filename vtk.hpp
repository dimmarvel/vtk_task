#pragma once
#include <stdio.h>
#include <string>
#include <cstdint>
#include <vector>

namespace app
{
    class tlv;

    enum descriminator : uint16_t
    {
        VMC = 0x96FB,
        POS = 0x97FB,
        UNKNOWN_DESCR = 0x00
    };

    std::string desc_to_string(descriminator t);

    class vtk_packet
    {
    public:
        vtk_packet() = default;
        vtk_packet(uint16_t lenth, descriminator descr, const std::vector<tlv>& packets);
        void write(FILE* file);
        void read(FILE* file);
        void info();

    private:
        uint16_t _length = 0;
        descriminator _descr = UNKNOWN_DESCR;
        std::vector<tlv> _tlv_packets{};     
        //TODO: list of parameters in tag
        //TODO: tvl in tvl
    };

    //Doesnt implemented
    class tcp_vtk_packet
    {
    private:
        uint8_t start;
        vtk_packet packet;
        uint16_t crc;
    };
}