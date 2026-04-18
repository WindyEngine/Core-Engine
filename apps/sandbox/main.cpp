#include <platform/filesystem.hpp>

#include <iostream>

int main()
{
    std::cout << Windy::Platform::Filesystem::normalizePath("./") << std::endl;
    return 0;
}