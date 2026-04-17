function(windy_add_executable NAME)
    cmake_parse_arguments(ARG
        ""
        ""
        "SOURCES;DEPS;INCLUDE_DIRS"
        ${ARGN}
    )

    # -----------------------------------------
    # Create executable
    # -----------------------------------------
    add_executable(${NAME} ${ARG_SOURCES})

    # -----------------------------------------
    # Include directories
    # -----------------------------------------
    if(ARG_INCLUDE_DIRS)
        target_include_directories(${NAME} PRIVATE ${ARG_INCLUDE_DIRS})
    endif()

    # -----------------------------------------
    # Dependencies
    # -----------------------------------------
    if(ARG_DEPS)
        target_link_libraries(${NAME} PRIVATE ${ARG_DEPS})
    endif()

    # -----------------------------------------
    # Platform system libraries
    # -----------------------------------------
    target_link_libraries(${NAME} PRIVATE ${WINDY_PLATFORM_SYSTEM_LIBS})

    # -----------------------------------------
    # Apply global settings
    # -----------------------------------------
    windy_apply_compiler_settings(${NAME})
    windy_apply_standard_settings(${NAME})

    # -----------------------------------------
    # Output organization
    # -----------------------------------------
    set_target_properties(${NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endfunction()