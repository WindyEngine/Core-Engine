# ---------------------------------------------
# libs/
# ---------------------------------------------
# Each library inside libs/ must expose a CMake target.

list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/cmake/dependencies"
)

# Asio
include(Asio)

# glfw
# if(EXISTS "${CMAKE_SOURCE_DIR}/libs/glfw/CMakeLists.txt")
#     add_subdirectory(${CMAKE_SOURCE_DIR}/libs/glfw)
# endif()

# # glm
# if(EXISTS "${CMAKE_SOURCE_DIR}/libs/glm/CMakeLists.txt")
#     add_subdirectory(${CMAKE_SOURCE_DIR}/libs/glm)
# endif()

# # stb
# if(EXISTS "${CMAKE_SOURCE_DIR}/libs/stb")
#     add_library(stb INTERFACE)
#     target_include_directories(stb INTERFACE
#         "${CMAKE_SOURCE_DIR}/libs/stb"
#     )
# endif()

# ---------------------------------------------
# Libs Alias Normalization
# ---------------------------------------------
if(TARGET asio::asio)
    add_library(Windy::asio ALIAS asio::asio)
endif()
# if(TARGET glfw)
#     add_library(Windy::glfw ALIAS glfw)
# endif()

# if(TARGET glm)
#     add_library(Windy::glm ALIAS glm)
# endif()

# if(TARGET stb)
#     add_library(Windy::stb ALIAS stb)
# endif()

# ---------------------------------------------
# SDKs
# ---------------------------------------------

# Vulkan
# if(DEFINED ENV{VULKAN_SDK})
#     set(VULKAN_PATH $ENV{VULKAN_SDK})

#     windy_add_sdk(VulkanSDK
#         INCLUDES "${VULKAN_PATH}/Include"
#         LIBS
#             "${VULKAN_PATH}/Lib/vulkan-1.lib"
#     )
# endif()

# DirectX12
# if(WINDY_PLATFORM_WINDOWS)
#     windy_add_sdk(DirectX12
#         LIBS
#             d3d12
#             dxgi
#     )
# endif()

# Metal
# if(WINDY_PLATFORM_MACOS)
#     windy_add_sdk(MetalSDK
#         LIBS
#             "-framework Metal"
#             "-framework MetalKit"
#     )
# endif()

# ---------------------------------------------
# Status output (debug visibility)
# ---------------------------------------------
message(STATUS "WindyEngine Dependencies Loaded")