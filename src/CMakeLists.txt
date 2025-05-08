add_subdirectory(resource_management)
#add_subdirectory(renderer)
add_subdirectory(platform)

add_executable(graphics main.cpp)

target_link_libraries(graphics resource_management platform)
