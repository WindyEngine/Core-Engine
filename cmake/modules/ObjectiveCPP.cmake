# ---------------------------------------------
# Enable Objective-C++
# ---------------------------------------------
enable_language(OBJCXX)

# ---------------------------------------------
# MacOS-Specific Configuration
# ---------------------------------------------
if(WINDY_PLATFORM_MACOS)
    # Ensure Objective-C++ files are treated correctly
    set(CMAKE_OBJCXX_STANDARD 20)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)

    # ARC (Automatic Reference Counting)
    option(WINDY_OBJCXX_ENABLE_ARC "Enable ARC for Objective-C++" ON)

    if(WINDY_OBJCXX_ENABLE_ARC)
        add_compile_options("-fobjc-arc")
    endif()
endif()

# ---------------------------------------------
# Helper: attach Objective-C++ sources
# ---------------------------------------------
function(windy_target_add_objcxx target)
    foreach(file IN LISTS ARGN)
        set_source_files_properties(${file} PROPERTIES LANGUAGE OBJCXX)
        target_sources(${target} PRIVATE ${file})
    endforeach()
endfunction()