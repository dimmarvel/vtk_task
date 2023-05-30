#include "helpers.hpp"
#include "tlv.hpp"
#include "vtk.hpp"
#include <iostream>
#include <bitset>

using namespace app;

int main()
{ 
    constexpr const char* dump_path = "./data.bin";
    
    FILE* file = fopen(dump_path, "wb");
    char str[] = "qwe";
    uint16_t counter = 0;

    std::cout << "----------------WRITE------------------" << std::endl;
    for(int i = 0; i < rnd(1, 10); ++i)
    {
        std::cout << counter++ << ". ";
        tlv p = create_rnd_tlv();
        descriminator descr = VMC;
        if(rnd(0,1)) descr = POS;
        std::string s = rand_str();
        vtk_packet vtk(1, descr, {{create_tag(rand_tag_bytes()), s.length(), s}});
        vtk.write(file);
        vtk.info();
    }

    descriminator descr = VMC;
    vtk_packet vtk(1, descr, {{static_cast<uint16_t>(std::bitset<16>(0b11111111'01010100).to_ulong()), 4, str}});
    vtk.write(file);
    vtk.info();

    std::cout << "----------------READ------------------" << std::endl;

    //packet.write(file);
    fclose(file);

    FILE* file2 = fopen(dump_path, "rb");

    counter = 0;
    while(!is_eof(file2))
    {
        std::cout << counter++ << ". ";
        vtk_packet new_packet;
        new_packet.read(file);
        new_packet.info();
    }

    fclose(file2);
    
    return 0;
}