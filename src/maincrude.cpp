#include "rendering.h"

#include <iostream>
#include <stdexcept>

int main()
{

        int test{};
        try
        {
                renderingMain();
        }
        catch (const std::exception &error)
        {
                std::cerr << "Something went wrong and is forced to shutdown.\n"
                          << error.what() << '\n';
                return -1;
        }

        std::cout << "It works\n";
        return 0;
}