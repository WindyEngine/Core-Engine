# # Project Properties
# cmake_minimum_required(VERSION 3.10)
# project(graphics)

# # Options
# option(ENABLE_CLANGD "Export Compile Command" OFF)
# option(ENABLE_OPENGL "Enable OpenGL backend" ON)
# set(ENABLE_OPENGL ${ENABLE_OPENGL} CACHE BOOL "Enable OpenGL support") 
# option(ENABLE_VULKAN "Enable Vulkan backend" ON)
# set(ENABLE_VULKAN ${ENABLE_VULKAN} CACHE BOOL "Enable Vulkan support")
# option(ENABLE_DIRECTX "Enable DirectX backend" OFF)
# set(ENABLE_DIRECTX ${ENABLE_DIRECTX} CACHE BOOL "Enable DirectX support") 
# option(ENABLE_METAL "Enable Metal backend" ON)
# set(ENABLE_METAL ${ENABLE_METAL} CACHE BOOL "Enable Metal support")

# # Settings
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ${ENABLE_CLANGD})
# set(CMAKE_CXX_STANDARD 17)

# # Includes
# include_directories(
#   "include"
#   "libs/headers"
#   ${WIN_HEADERS}
#   ${Vulkan_INCLUDE_DIR}
#   "$ENV{VULKAN_SDK}/Include"
# )

# # Globals
# set(METAL_LIBS "-framework Cocoa" "-framework QuartzCore" "-framework Metal")
# set(OPENGL_LIBS glfw glad)
# set(VULKAN_LIBS ${Vulkan_LIBRARIES})
# set(DIRECTX_LIBS ON)
# set(VULKAN_SDK $ENV{VULKAN_SDK})


# # Definitions
# if (ENABLE_OPENGL)
#   message("Compiling with OpenGL")
#   add_definitions(-DENGINE_COMPILE_OPENGL)
# endif()
# if (ENABLE_VULKAN)
#   message("Compiling with Vulkan")
#   add_definitions(-DENGINE_COMPILE_VULKAN)
# endif()
# if (ENABLE_DIRECTX)
#   message("Compiling with DirectX")
#   add_definitions(-DENGINE_COMPILE_DIRECTX)
# endif()
# if (ENABLE_METAL)
#   message("Compiling with Metal")
#   add_definitions(-DENGINE_COMPILE_METAL)
# endif()

# # Templates
# file(COPY ${CMAKE_SOURCE_DIR}/templates DESTINATION ${CMAKE_BINARY_DIR})
# add_compile_definitions(TEMPLATES_PATH="${CMAKE_BINARY_DIR}/templates")

# # Main Binary
# add_subdirectory(libs)
# add_subdirectory(src)


cmake_minimum_required(VERSION 3.25)

project(WindyEngine
  VERSION 0.1.0
  LANGUAGES C CXX OBJC OBJCXX
)

# ------------------------------------------------------------------
# Global
# ------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------
# Internal CMake
# ------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/cmake/modules"
)

include(PlatformDetection)
include(PlatformConfig)
include(CompilerSettings)
include(StandardSettings)
include(ObjectiveCPP)

include(AddLibrary)
include(AddExecutable)
include(AddSDK)
include(Dependencies)

# ------------------------------------------------------------------
# External Libraries
# ------------------------------------------------------------------
#add_subdirectory(libs)

# ------------------------------------------------------------------
# Source Files
# ------------------------------------------------------------------
add_subdirectory(src)

# ------------------------------------------------------------------
# Applications
# ------------------------------------------------------------------
add_subdirectory(apps)