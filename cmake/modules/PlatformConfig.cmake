# ---------------------------------------------
# System Libraries
# ---------------------------------------------
set(WINDY_SYSTEM_LIBS "")

# ---------------------------------------------
# Windows Configuration
# ---------------------------------------------
if(WINDY_PLATFORM_WINDOWS)

    # Core Windows system libraries
    list(APPEND WINDY_SYSTEM_LIBS
        user32
        gdi32
        shell32
        ole32
        uuid
        advapi32
    )

    add_compile_definitions(
        WINDY_PLATFORM_WINDOWS=1
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )

endif()

# ---------------------------------------------
# Linux Configuration
# ---------------------------------------------
if(WINDY_PLATFORM_LINUX)

    list(APPEND WINDY_SYSTEM_LIBS
        pthread
        dl
        m
    )

    add_compile_definitions(
        WINDY_PLATFORM_LINUX=1
    )

endif()

# ---------------------------------------------
# MacOS Configuration
# ---------------------------------------------
if(WINDY_PLATFORM_MACOS)

    list(APPEND WINDY_SYSTEM_LIBS
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework QuartzCore"
    )

    add_compile_definitions(
        WINDY_PLATFORM_MACOS=1
    )

endif()

# ---------------------------------------------
# Architecture Definitions
# ---------------------------------------------
if(WINDY_ARCH_X86_64)
    add_compile_definitions(WINDY_ARCH_X86_64=1)
elseif(WINDY_ARCH_ARM64)
    add_compile_definitions(WINDY_ARCH_ARM64=1)
endif()

# ---------------------------------------------
# Export Helper Variable
# ---------------------------------------------
set(WINDY_PLATFORM_SYSTEM_LIBS ${WINDY_SYSTEM_LIBS})

# ---------------------------------------------
# Status output
# ---------------------------------------------
message(STATUS "WindyEngine System Libs: ${WINDY_PLATFORM_SYSTEM_LIBS}")