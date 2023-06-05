#include "app.hpp"
#include "tlv.hpp"
#include "parser.hpp"
#include <iostream>

namespace app
{
    application::application(const std::string& path)
    :
    _path(path),
    _tlv_parser(std::make_shared<parser::tlv_parser>(_path))
    {
        write_test_data();
    }
    
    void application::write_test_data()
    {
        FILE* file = fopen(_path.c_str(), "wb");
        //tag -> 01011111 11011111 01011111 len -> 00000010  -> 2 0x5fdf5f "da" 2 0x5fdf5f "ds"
        //std::vector<uint8_t> d = {0x5F, 0xDF, 0x5F, 0x2, 0x64, 0x73, 0x5F, 0xDF, 0x5F, 0x2, 0x64, 0x73 };
        std::vector<uint8_t> d = {0x6F, 0x24, 0x84, 0x0E, 0x31, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31, 0xA5, 0x12, 0x88, 0x01, 0x01, 0x5F, 0x2D, 0x08, 0x65, 0x73, 0x65, 0x6E, 0x70, 0x74, 0x66, 0x72, 0x9F, 0x11, 0x01, 0x01}; // 6F24840E315041592E5359532E4444463031A5128801015F2D086573656E707466729F110101
        std::fwrite(&d[0], sizeof(uint8_t), d.size(), file);
        std::fclose(file);
    }

    void application::start()
    {
        std::cout << "Application started, test:" << std::endl;
        std::vector<tlv> tlvs = _tlv_parser->parse();

        for(auto& it : tlvs)
            it.info();
    } 
}
