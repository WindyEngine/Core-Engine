/** 
 * @file host.hpp
 * @brief Provides compile-time detection of the underlying host platform and hardware.
*/

#pragma once

#include <string_view>


/**
 * @namespace Windy::Platform
 * @brief Contains low-level hardware abstraction and platform-specific logic
 */
namespace Windy::Platform {
    /**
     * @enum OS
     * @brief Supported Operating Systems
     */
    enum class OS {
        Windows,    ///< Microsoft Windows
        Linux,      ///< Generic Linux Distribution
        MacOS,      ///< Apple macOS
        Unknown     ///< Unsupported or undetected OS
    };

    /**
     * @enum CPU
     * @brief Supported Instruction Set Architecture (ISA)
     */
    enum class CPU {
        x86_64,     ///< 64-bit Intel/AMD architecture
        Arm64,      ///< 64-bit ARM architecture (Apple Silicon, mobile, etc)
        Unknown     ///< Unsupported or undetected architecture
    };

    /**
     * @class Host
     * @brief A static utility class providing information about the build target.
     * * The Host class bridges CMake build definitions to C++ constexpr constants,
     * allowing for zero-overhead branching using 'if constexpr'.
     */
    class Host {
    public:
        /// Static-only class: instantiation is prohibited.
        Host() = delete;

        /**
         * @brief The detected Operating System of the target build
         */
        #if defined(WINDY_PLATFORM_WINDOWS)
        static constexpr OS os = OS::Windows;
        #elif defined(WINDY_PLATFORM_LINUX)
        static constexpr OS os = OS::Linux;
        #elif defined(WINDY_PLATFORM_MACOS)
        static constexpr OS os = OS::MacOS;
        #else
        static constexpr OS os = OS::Unknown;
        #endif

        /**
         * @brief The detected CPU architecture of the target build
         */
        #if defined(WINDY_ARCH_X86_64)
            static constexpr CPU cpu = CPU::x86_64;
        #elif defined(WINDY_ARCH_ARM64)
            static constexpr CPU cpu = CPU::Arm64;
        #else
            static constexpr CPU cpu = CPU::Unknown;
        #endif

        /**
         * @brief The memory address width in bits
         */
        #ifdef WINDY_ARCH_64
        static constexpr int pointerSize = 64;
        #else
        static constexpr int pointerSize = 32;
        #endif

        /**
         * @brief Returns a string representation of the current OS.
         * @return std::string_view containing the OS name
         */
        static constexpr std::string_view GetOSName() {
            switch (os) {
                case OS::Windows: return "Windows";
                case OS::Linux:   return "Linux";
                case OS::MacOS:   return "MacOS";
                default:          return "Unknown";
            }
        }

        /**
         * @brief Returns a string representation of the current CPU architecture.
         * @return std::string_view containing the architecture name.
         */
        static constexpr std::string_view GetArchName() {
            switch (cpu) {
                case CPU::x86_64: return "x86_64";
                case CPU::Arm64:  return "Arm64";
                default:           return "Unknown";
            }
        }
    };
}