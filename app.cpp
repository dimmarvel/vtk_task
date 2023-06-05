#include "app.hpp"
#include "tlv.hpp"
#include "vtk.hpp"
#include <iostream>

namespace app
{
    application::application(const std::string& path)
    :
    _path(path),
    _vtk_parser(std::make_unique<parser::vtk_parser>(path))
    {
        write_test_data();
    }
    
    void application::write_test_data()
    {
        FILE* file = fopen(_path.c_str(), "wb");
        //tag -> 01011111 11011111 01011111 len -> 00000010  -> 2 0x5fdf5f "da" 2 0x5fdf5f "ds"
        //std::vector<uint8_t> d = {0x5F, 0xDF, 0x5F, 0x2, 0x64, 0x73, 0x5F, 0xDF, 0x5F, 0x2, 0x64, 0x73 };
        std::vector<uint16_t> vtk1 = {0x0026, 0x96FB};
        std::fwrite(&vtk1[0], sizeof(uint16_t), vtk1.size(), file);
        std::vector<uint8_t> tlv1 = {0x6F, 0x24, 0x84, 0x0E, 0x31, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31, 0xA5, 0x12, 0x88, 0x01, 0x01, 0x5F, 0x2D, 0x08, 0x65, 0x73, 0x65, 0x6E, 0x70, 0x74, 0x66, 0x72, 0x9F, 0x11, 0x01, 0x01}; // 6F24840E315041592E5359532E4444463031A5128801015F2D086573656E707466729F110101
        std::fwrite(&tlv1[0], sizeof(uint8_t), tlv1.size(), file);
        std::fclose(file);
    }

    void application::start()
    {
        std::cout << "Application started, test:" << std::endl;
        std::vector<vtk> vtks = std::move(_vtk_parser->parse());

        for(auto& it : vtks)
            std::cout << it.info() << std::endl;
    } 
}
