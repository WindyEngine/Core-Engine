# ---------------------------------------------
# OS Detection
# ---------------------------------------------
if(WIN32)
    set(WINDY_PLATFORM_WINDOWS TRUE)
    set(WINDY_PLATFORM_LINUX   FALSE)
    set(WINDY_PLATFORM_MACOS   FALSE)

elseif(APPLE)
    set(WINDY_PLATFORM_WINDOWS FALSE)
    set(WINDY_PLATFORM_LINUX   FALSE)
    set(WINDY_PLATFORM_MACOS   TRUE)

elseif(UNIX)
    set(WINDY_PLATFORM_WINDOWS FALSE)
    set(WINDY_PLATFORM_LINUX   TRUE)
    set(WINDY_PLATFORM_MACOS   FALSE)

else()
    message(FATAL_ERROR "Unsupported platform detected")
endif()

# ---------------------------------------------
# Architecture Detection
# ---------------------------------------------
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WINDY_ARCH_64 TRUE)
    set(WINDY_ARCH_32 FALSE)
else()
    set(WINDY_ARCH_64 FALSE)
    set(WINDY_ARCH_32 TRUE)
endif()

# ---------------------------------------------
# CPU Family Detection
# ---------------------------------------------
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(WINDY_ARCH_X86_64 TRUE)
    set(WINDY_ARCH_ARM64  FALSE)

elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(WINDY_ARCH_X86_64 FALSE)
    set(WINDY_ARCH_ARM64  TRUE)
else()
    set(WINDY_ARCH_X86_64 FALSE)
    set(WINDY_ARCH_ARM64  FALSE)
endif()

# ---------------------------------------------
# Normalized Platform String
# ---------------------------------------------
if(WINDY_PLATFORM_WINDOWS)
    set(WINDY_PLATFORM_NAME "windows")
elseif(WINDY_PLATFORM_LINUX)
    set(WINDY_PLATFORM_NAME "linux")
elseif(WINDY_PLATFORM_MACOS)
    set(WINDY_PLATFORM_NAME "macos")
else()
    set(WINDY_PLATFORM_NAME "unknown")
endif()

# ---------------------------------------------
# Status output
# ---------------------------------------------
message(STATUS "WindyEngine Platform: ${WINDY_PLATFORM_NAME}")