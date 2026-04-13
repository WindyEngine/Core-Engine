add_subdirectory(resource_management)
add_subdirectory(renderer)
add_subdirectory(platform)

add_executable(graphics main.cpp)

if (ENABLE_METAL)
  file(GLOB_RECURSE OBJ_SRC "*.mm")
  target_sources(graphics ${OBJ_SRC})
  target_link_libraries(graphics ${METAL_LIBS})
elseif(ENABLE_OPENGL)
  target_link_libraries(graphics ${OPENGL_LIBS})
elseif(ENABLE_VULKAN)
  target_link_libraries(graphics ${VULKAN_LIBS})
elseif(ENABLE_DIRECTX)
  target_sources(graphics ${DIRECTX_LIBS})
endif()

target_link_libraries(graphics renderer resource_management platform)

add_custom_command(
    TARGET graphics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/templates"
            "${CMAKE_BINARY_DIR}/templates"
)
