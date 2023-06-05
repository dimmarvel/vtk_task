#pragma once
#include <string>
#include <memory>
#include "vtk_parser.hpp"

namespace app
{
    class application
    {
    public:
        explicit application(const std::string& path);
        ~application() = default;

        void start();
        void write_test_data();

        application(const application& copy) = delete;
        application& operator=(const application& copy) = delete;

    private:
        std::string _path;
        std::unique_ptr<parser::vtk_parser> _vtk_parser;
    };
}
