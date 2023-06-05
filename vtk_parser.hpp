#pragma once
#include <string>
#include <vector>
#include <memory>

namespace app
{
    class tlv;
    class length;
    class vtk;
}

namespace app::parser
{
    class tlv_parser;

    class vtk_parser
    {
    public:
        explicit vtk_parser(const std::string& path);
        ~vtk_parser();

        vtk_parser(const vtk_parser& copy) = delete;
        vtk_parser& operator=(const vtk_parser& copy) = delete;

        std::vector<vtk> parse();
        
    private:
        uint16_t read_uint16();
        std::vector<tlv> read_app_msg(uint16_t len);
        bool is_eof();

    private:
        std::unique_ptr<tlv_parser> _tlv_parser;
        FILE* _file;
        size_t _file_sz = 0;
    };
}