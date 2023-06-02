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
        std::vector<uint8_t> d  {    
                                    0x5F, 0xDF, 0x5F, 0x2, 0x64, 0x73,
                                    0x5F, 0xDF, 0x5F, 0x2, 0x64, 0x73
                                };
        std::fwrite(&d[0], sizeof(uint8_t), d.size(), _file);
        std::fclose(_file);
    }

    void application::start()
    {
        std::cout << "Application started, tests:" << std::endl;

        _file = fopen(_path.c_str(), "rb");
        tlv_parser tp(_file);
        std::vector<tlv> tlvs = tp.parse();

        std::ios init(NULL);
        init.copyfmt(std::cout);
        for(auto& it : tlvs)
        {
            std::cout << "tlv\n{\n";
            std::cout << "\ttag: 0x" << std::hex << it.get_tag_number() << std::endl;
            std::cout.copyfmt(init);
            std::cout << "\tlength: " << it.len.size << std::endl;
            std::cout << "\tvalue: " << std::string(it.value.begin(), it.value.end()) << std::endl;
            std::cout << "}\n";
        }
        std::fclose(_file);
    } 
}
