#include "tlv.hpp"
#include "vtk.hpp"
#include "vtk_parser.hpp"
#include "tlv_parser.hpp"
#include <stdexcept>

namespace app::parser
{
    #define SAFE_READ(data, byte_size, size, file_size, file)       \
        fpos_t pos;                                                 \
        fgetpos(file, &pos);                                        \
        if((file_size - pos.__pos) < (size))                        \
            throw std::runtime_error(                               \
                "Not enought data for read(" +                      \
                std::to_string((file_size - pos.__pos)) +           \
                " < " + std::to_string(size) + ")");                \
        if(!std::fread(data, byte_size, size, file))                \
            throw std::runtime_error("Fail read file")

    vtk_parser::vtk_parser(const std::string& path)
    :
    _tlv_parser(std::make_unique<tlv_parser>())
    {
        _file = fopen(path.c_str(), "rb"); // Check if cant open then throw?
        fseek(_file, 0L, SEEK_END);
        _file_sz = ftell(_file);
        rewind(_file);
    }

    vtk_parser::~vtk_parser()
    {
        if(_file != nullptr)
            fclose(_file);
    }

    std::vector<vtk> vtk_parser::parse()
    {
        std::vector<vtk> vtks;

        while(!is_eof())
        {
            vtk v;
            v.len = read_uint16();
            v.descrim = read_uint16();
            v.tlvs = read_app_msg(v.len);
            vtks.emplace_back(std::move(v));
        }
        return vtks;
    }

    uint16_t vtk_parser::read_uint16()
    {
        uint16_t sz = 0;
        SAFE_READ(&sz, sizeof(uint16_t), 1, _file_sz, _file);
        return sz;
    }

    std::vector<tlv> vtk_parser::read_app_msg(uint16_t len)
    {
        std::vector<uint8_t> data;
        data.reserve(len);
        data.resize(len);
        SAFE_READ(&data[0], sizeof(uint8_t), len, _file_sz, _file);
        return _tlv_parser->parse(std::move(data));
    }

    bool vtk_parser::is_eof()
    {
        getc(_file);
        if(feof(_file)) return true;
        fseek(_file, -1, SEEK_CUR);
        return false;
    }

}