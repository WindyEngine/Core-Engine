
/**
 * @file file.hpp
 * @brief Declares file utility functions for the resource management system.
 *
 * This file provides utility functions for common file operations used throughout
 * the resource management module, such as reading the contents of a file into a string.
 */

#pragma once
#include <string>


namespace engine::resource_management {

/**
 * @brief Reads the entire contents of a file and returns it as a string.
 *
 * This function opens a file located at the specified path and reads its entire contents
 * into a `std::string`, which it returns. It is typically used for reading source code files,
 * shader files, configuration files, or any other text-based resources.
 *
 * @param path The filesystem path to the file to read. This should be an absolute or relative path.
 * @return A `std::string` containing the full contents of the file.
 */
std::string readFile(std::string path);

}
