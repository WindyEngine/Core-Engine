
# Collect core sources
set(CORE_SOURCES)

# ---------------------------------------------
# Core
# ---------------------------------------------
list(APPEND CORE_SOURCES
    engine.cpp
    
    module/registry.cpp
    module/resolver.cpp
    module/kernel.cpp
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
# Core library target
# ---------------------------------------------
windy_add_library(core
    SOURCES ${CORE_SOURCES}

    INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/include
)