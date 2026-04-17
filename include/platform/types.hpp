/** 
 * @file types.hpp
 * @brief Defines fundamental primitive type aliases used across the engine.
*/

#pragma once

#include <cstdint>

/**
 * @namespace Windy
 * @brief The core namespace for the Windy Engine
 */
namespace Windy {
    /**
     * @brief Represents an 8-bit unsigned byte of data
     */
    using Byte = std::uint8_t;

    /**
     * @brief The primary floating-point type for engine-wide calculations
     */
    using Real = double;
}