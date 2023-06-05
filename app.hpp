#pragma once
#include <string>
#include <memory>

namespace app::parser
{
    class tlv_parser;
}

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
        std::shared_ptr<parser::tlv_parser> _tlv_parser;
    };
}
