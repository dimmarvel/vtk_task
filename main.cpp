#include "app.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    try
    {
        app::application app("data.bin");
        app.start();
    }
    catch(const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
