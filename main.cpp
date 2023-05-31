#include "helpers.hpp"
#include "tlv.hpp"
#include "vtk.hpp"
#include <iostream>
#include <bitset>

using namespace app;

void tests()
{
    constexpr const char* dump_path = "./data.bin";
    
    FILE* file = fopen(dump_path, "wb");
    std::vector<uint8_t> str = rand_tlv_msg();
    uint16_t counter = 0;
    descriminator descr = VMC;

    std::cout << "----------------WRITE------------------" << std::endl;
    for(int i = 0; i < rnd(8, 32); ++i)
    {
        std::cout << counter++ << ". ";
        tlv p = create_rnd_tlv();
        str = rand_tlv_msg();
        descr = VMC;
        if(rnd(0,1)) 
        {
            str = rand_str();
            descr = POS;
        }
        vtk_packet vtk(1, descr, {{ create_tag(rand_tag_bytes()), 
                                    static_cast<uint16_t>(str.size()), 
                                    str}});
        vtk.write(file);
        vtk.info();
    }

    //Create two byte tag, just for test
    vtk_packet vtk(1, descr, {{ static_cast<uint16_t>(std::bitset<16>(0b11111111'01010100).to_ulong()), 
                                static_cast<uint16_t>(str.size()), str}});
    vtk.write(file);
    std::cout << counter++ << ". ";
    vtk.info();
    fclose(file);

    std::cout << "----------------READ------------------" << std::endl;

    file = fopen(dump_path, "rb");

    counter = 0;
    while(!is_eof(file))
    {
        std::cout << counter++ << ". ";
        vtk_packet new_packet;
        new_packet.read(file);
        new_packet.info();
    }

    fclose(file);
}

int main()
{ 
    try
    {
        tests();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}