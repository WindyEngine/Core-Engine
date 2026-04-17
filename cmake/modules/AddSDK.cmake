function(windy_add_sdk NAME)
    cmake_parse_arguments(ARG
        ""
        "PATH"
        "INCLUDES;LIBS;DEFINES"
        ${ARGN}
    )

    # -----------------------------------------
    # Create interface target
    # -----------------------------------------
    add_library(${NAME} INTERFACE)

    # -----------------------------------------
    # Include directories
    # -----------------------------------------
    if(ARG_INCLUDES)
        target_include_directories(${NAME} INTERFACE ${ARG_INCLUDES})
    elseif(ARG_PATH)
        target_include_directories(${NAME} INTERFACE "${ARG_PATH}/include")
    endif()

    # -----------------------------------------
    # Link libraries / frameworks
    # -----------------------------------------
    if(ARG_LIBS)
        target_link_libraries(${NAME} INTERFACE ${ARG_LIBS})
    endif()

    # -----------------------------------------
    # Compile definitions
    # -----------------------------------------
    if(ARG_DEFINES)
        target_compile_definitions(${NAME} INTERFACE ${ARG_DEFINES})
    endif()

    # -----------------------------------------
    # Namespacing
    # -----------------------------------------
    add_library(Windy::${NAME} ALIAS ${NAME})
endfunction()