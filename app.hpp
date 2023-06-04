#pragma once
#include <string>
#include <cstdio>

namespace app
{
    class application
    {
    public:
        explicit application(const std::string& path);
        ~application() = default;

        void start();

        application(const application& copy) = delete;
        application& operator=(const application& copy) = delete;
        
    private:
        std::string _path;
        FILE* _file;
    };
}
