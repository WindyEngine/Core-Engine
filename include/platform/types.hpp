/** 
 * @file types.hpp
 * @brief Defines fundamental primitive type aliases used across the engine.
*/

#pragma once

#include <cstdint>
#include <string>

/**
 * @namespace Windy::Platform
 * @brief Contains low-level hardware abstraction and platform-specific logic
 */
namespace Windy::Platform {
    /**
     * @brief The primary floating-point type for engine-wide calculations
     */
    using Real = double;

    /**
     * @brief The default string type for text and path manipulation
     */
    using String = std::string;
}