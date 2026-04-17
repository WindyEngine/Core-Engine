#include <platform/platform.hpp>

#include <iostream>


int main()
{
    std::cout << Windy::Platform::Host::GetOSName() << std::endl;
    return 0;
}