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