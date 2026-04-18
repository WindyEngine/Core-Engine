
# Collect platform sources
set(PLATFORM_SOURCES)

# ---------------------------------------------
# Core
# ---------------------------------------------
list(APPEND PLATFORM_SOURCES
    filesystem.cpp
    network.cpp
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