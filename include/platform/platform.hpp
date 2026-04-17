#pragma once

#include <string_view>


namespace Windy::Platform {
    enum class OS {
        Windows, Linux, MacOS, Unknown
    };

    
    enum class CPU {
        x86_64, Arm64, Unknown
    };


    class Host {
    public:
        Host() = delete;


        #if defined(WINDY_PLATFORM_WINDOWS)
        static constexpr OS os = OS::Windows;
        #elif defined(WINDY_PLATFORM_LINUX)
        static constexpr OS os = OS::Linux;
        #elif defined(WINDY_PLATFORM_MACOS)
        static constexpr OS os = OS::MacOS;
        #else
        static constexpr OS os = OS::Unknown;
        #endif


        #if defined(WINDY_ARCH_X86_64)
            static constexpr CPU cpu = CPU::x86_64;
        #elif defined(WINDY_ARCH_ARM64)
            static constexpr CPU cpu = CPU::Arm64;
        #else
            static constexpr CPU cpu = CPU::Unknown;
        #endif


        #ifdef WINDY_ARCH_64
        static constexpr int pointerSize = 64;
        #else
        static constexpr int pointerSize = 32;
        #endif


        static constexpr std::string_view GetOSName() {
            switch (os) {
                case OS::Windows: return "Windows";
                case OS::Linux:   return "Linux";
                case OS::MacOS:   return "MacOS";
                default:          return "Unknown";
            }
        }


        static constexpr std::string_view GetArchName() {
            switch (cpu) {
                case CPU::x86_64: return "x86_64";
                case CPU::Arm64:  return "Arm64";
                default:           return "Unknown";
            }
        }
    };
}