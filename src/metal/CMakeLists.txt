cmake_minimum_required(VERSION 3.10)

# Set project name and version
project(MetalApp VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Set the source files
set(SOURCES
    main.mm
    metal_window.mm
    metal_renderer.mm
    MetalView.mm
)

# Set the header files
set(HEADERS
    metal_window.hpp
)

# Find required frameworks
find_library(COCOA_FRAMEWORK Cocoa)
find_library(QUARTZCORE_FRAMEWORK QuartzCore)
find_library(METAL_FRAMEWORK Metal)

# Check if libraries were found
if (NOT COCOA_FRAMEWORK)
    message(FATAL_ERROR "Cocoa framework not found!")
endif()

if (NOT QUARTZCORE_FRAMEWORK)
    message(FATAL_ERROR "QuartzCore framework not found!")
endif()

if (NOT METAL_FRAMEWORK)
    message(FATAL_ERROR "Metal framework not found!")
endif()

# Add the executable
add_executable(MetalApp
    main.mm
    renderer/metal_renderer.mm
    platform/metal_window.mm
    platform/MetalView.mm
)

# Link required frameworks
target_link_libraries(MetalApp
    ${COCOA_FRAMEWORK}
    ${QUARTZCORE_FRAMEWORK}
    ${METAL_FRAMEWORK}
)

# Set additional include directories
target_include_directories(MetalApp PRIVATE
    renderer
    platform
)


# If you need to use the Metal framework's lib (though it is usually automatically linked)
target_link_libraries(MetalApp
    "-framework Metal"
)
