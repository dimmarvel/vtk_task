#include "vtk.hpp"
#include "tlv.hpp"
#include <bitset>
#include <iostream>

namespace app
{
    vtk_packet::vtk_packet(uint16_t length, descriminator descr, const std::vector<tlv>& packets)
    :
    _length(length),
    _descr(descr),
    _tlv_packets(packets)
    {};

    void vtk_packet::write(FILE* file)
    {
        fwrite(&_length, sizeof(uint16_t), 1, file);
        fwrite(&_descr, sizeof(uint16_t), 1, file);
        for(auto& pkt : _tlv_packets)
            pkt.write(file);
    }

    void vtk_packet::read(FILE* file)
    {
        tlv packet;
        fread(&_length, sizeof(uint16_t), 1, file);
        fread(&_descr, sizeof(uint16_t), 1, file);
        for(int i = 0; i < _length; ++i)
        {
            packet.read(file);
            _tlv_packets.push_back(std::move(packet));
        }
    }

    void vtk_packet::info()
    {
        std::cout 
            << "VTK {" 
            << "length{" + std::to_string(_length) << "}, "
            << "descriminator{" + desc_to_string(_descr) + "}, "
            << "app msg{";
        for(auto& it : _tlv_packets)
            std::cout << it.info() << " ";
        std::cout << "} " << std::endl;        
    }

    std::string desc_to_string(descriminator t)
    {
        switch (t)
        {
            case VMC: return "VMC";
            case POS: return "POS";
            default:
                return "unknown";
        }
    }

}