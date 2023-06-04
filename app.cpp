#include "app.hpp"
#include "tlv.hpp"
#include <iostream>

namespace app
{
    application::application(const std::string& path)
    :
    _path(path)
    {
        _file = fopen(_path.c_str(), "wb");
        //tag -> 01011111 11011111 01011111 len -> 10000010 00000001 10110011 -> 435 0x5fdf5f
        //std::vector<uint8_t> d { 0x5F, 0xDF, 0x5F, 0x82, 0x01, 0xB3, 0x73, 0x64};
        //tag -> 01011111 11011111 01011111 len -> 01101011  -> 107 0x5fdf5f rndData
        //std::vector<uint8_t> d { 0x5F, 0xDF, 0x5F, 0x6B, 0x6B, 0x6B, 0xF0, 0xA0, 0x01 };
        //tag -> 01011111 11011111 01011111 len -> 00000010  -> 2 0x5fdf5f "da" 2 0x5fdf5f "ds"
        // std::vector<uint8_t> d  {    
        //                             0x5F, 0xDF, 0x5F, 0x2, 0x64, 0x73,
        //                             0x5F, 0xDF, 0x5F, 0x2, 0x64, 0x73
        //                         };
        // std::vector<uint8_t> d  { 0x6F, 0x04, 0x4F, 0x02, 0x64, 0x73}; //6F044F026473
        const char* tmp = "\x6F\x24\x84\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31\xA5\x12\x88\x01\x01\x5F\x2D\x08\x65\x73\x65\x6E\x70\x74\x66\x72\x9F\x11\x01\x01";
        std::vector<uint8_t> d(tmp, tmp + 39); // 6F24840E315041592E5359532E4444463031A5128801015F2D086573656E707466729F110101
        std::fwrite(&d[0], sizeof(uint8_t), d.size(), _file);
        std::fclose(_file);
    }

    void application::start()
    {
        std::cout << "Application started, tests:" << std::endl;

        _file = fopen(_path.c_str(), "rb");
        tlv_parser tp(_file);
        std::vector<tlv> tlvs = tp.parse();

        for(auto& it : tlvs)
            it.info();
        std::fclose(_file);
    } 
}
