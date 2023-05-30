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

    std::string vtk_stringify(uint8_t id)
    {
        struct descr_params
        {
            uint8_t id;
            std::string descr;
        } d_params[] = {
            { 0x01, "Message name"},
            { 0x03, "Operation number"}, 
            { 0x04, "Amount in minor currency unit"}, 
            { 0x05, "Keepalive interval in seconds"}, 
            { 0x06, "Operation timeout in seconds"}, 
            { 0x07, "Event name"},
            { 0x08, "Event number"}, 
            { 0x09, "Product id"}, 
            { 0x0A, "QR-Code data"}, 
            { 0x0B, "TCP/IP destination"}, 
            { 0x0C, "Outgoing byte counter"}, 
            { 0x0D, "Simple data block"}, 
            { 0x0E, "Confirmable data block"}, 
            { 0x0F, "Product name"}, 
            { 0x10, "POS management data"}, 
            { 0x11, "Local time"}, 
            { 0x12, "System information"}, 
            { 0x13, "Banking receipt"}, 
            { 0x14, "Display time in milliseconds"}, 
            { 0x15, "Image name"},
            { 0x16, "Image flags"}, 
            { 0x17, "Display text"}, 
            { 0x18, "Coordinates and color"}, 
            { 0x19, "Host Id"}
        };

        for (int arg = 0; d_params[arg].id; arg++) {
            if (d_params[arg].id == id) {
                return d_params[arg].descr;
            }
        }

        return "Unknown ID";
    }
}