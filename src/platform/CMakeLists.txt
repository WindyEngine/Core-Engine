# file(GLOB SRC "*.cpp")
# add_library(platform ${SRC})

# if (ENABLE_METAL)
#   file(GLOB_RECURSE OBJ_SRC "*.mm")
#   target_sources(platform PRIVATE ${OBJ_SRC})
#   target_link_libraries(platform ${METAL_LIBS})
# endif()
# if(ENABLE_OPENGL)
#   target_link_libraries(platform ${OPENGL_LIBS})
# endif()
# if(ENABLE_VULKAN)
#   if(WIN32)
    
#     link_directories(platform "${VULKAN_SDK}/Lib")
#     target_link_libraries(platform "${VULKAN_SDK}/Lib/vulkan-1.lib")
#   elseif(UNIX)
#     target_link_libraries(platform vulkan)
#   endif()
# endif()


# Collect platform sources
set(PLATFORM_SOURCES)

# ---------------------------------------------
# Core
# ---------------------------------------------
list(APPEND PLATFORM_SOURCES
    platform_detection.cpp
)

# ---------------------------------------------
# OS-specific
# ---------------------------------------------
# if(WINDY_PLATFORM_WINDOWS)
#     list(APPEND PLATFORM_SOURCES
#         src/windows/window_win32.cpp
#         src/windows/file_win32.cpp
#         src/windows/thread_win32.cpp
#     )
# endif()

# ---------------------------------------------
# Platform library target
# ---------------------------------------------
windy_add_library(platform
    SOURCES ${PLATFORM_SOURCES}

    INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/include
)