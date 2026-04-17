function(windy_add_library NAME)
    cmake_parse_arguments(ARG
        "STATIC;SHARED;HEADER_ONLY"
        ""
        "SOURCES;PUBLIC_DEPS;PRIVATE_DEPS;INCLUDE_DIRS"
        ${ARGN}
    )

    # -----------------------------------------
    # Library Type Resolution
    # -----------------------------------------
    if(ARG_HEADER_ONLY)
        add_library(${NAME} INTERFACE)
    elseif(ARG_SHARED)
        add_library(${NAME} SHARED ${ARG_SOURCES})
    else()
        add_library(${NAME} STATIC ${ARG_SOURCES})
    endif()

    # -----------------------------------------
    # Include Directories
    # -----------------------------------------
    if(ARG_INCLUDE_DIRS)
        if(ARG_HEADER_ONLY)
            target_include_directories(${NAME} INTERFACE ${ARG_INCLUDE_DIRS})
        else()
            target_include_directories(${NAME} PUBLIC ${ARG_INCLUDE_DIRS})
        endif()
    endif()

    # -----------------------------------------
    # Dependencies
    # -----------------------------------------
    if(ARG_PUBLIC_DEPS)
        target_link_libraries(${NAME} PUBLIC ${ARG_PUBLIC_DEPS})
    endif()

    if(ARG_PRIVATE_DEPS AND NOT ARG_HEADER_ONLY)
        target_link_libraries(${NAME} PRIVATE ${ARG_PRIVATE_DEPS})
    endif()

    # -----------------------------------------
    # Platform system libraries
    # -----------------------------------------
    if(NOT ARG_HEADER_ONLY)
        target_link_libraries(${NAME} PRIVATE ${WINDY_PLATFORM_SYSTEM_LIBS})
    endif()

    # -----------------------------------------
    # Apply global settings
    # -----------------------------------------
    if(NOT ARG_HEADER_ONLY)
        windy_apply_compiler_settings(${NAME})
        windy_apply_standard_settings(${NAME})
    endif()

    # -----------------------------------------
    # Namespacing
    # -----------------------------------------
    add_library(Windy::${NAME} ALIAS ${NAME})
endfunction()