/** 
 * @file types.hpp
 * @brief Defines fundamental primitive type aliases used across the engine.
*/

#pragma once

#include <platform/types.hpp>
#include <filesystem>

/**
 * @namespace Windy::Platform
 * @brief Contains low-level hardware abstraction and platform-specific logic
 */
namespace Windy::Platform {
    /**
     * @class Filesystem
     * @brief A utility class for path manipulation and file operations
     */
    class Filesystem {
    public:
        /// Static-only class: instantiation is prohibited.
        Filesystem() = delete;

        /**
         * @brief Checks if a given path exists on the disk
         * @param path The path to check
         */
        static bool exists(const String& path);

        /**
         * @brief Returns the platform-specific directory separator.
         * @return '/' on Unix-like system, '\' on Windows.
         */
        static constexpr char getSeparator() {
            #if defined(WINDY_PLATFORM_WINDOWS)
                return '\\';
            #else
                return '/';
            #endif
        }

        /**
         * @brief Normalizes a path to use the current platform's separators.
         * @param path The raw path string.
         * @return A normalized path string.
         */
        static String normalizePath(const String& path);
    };
}