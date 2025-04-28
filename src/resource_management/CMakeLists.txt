add_subdirectory(resources)

file (GLOB SRC "*.cpp")

add_library(resource_management ${SRC})

target_link_libraries(resource_management PRIVATE resources)
