file (GLOB_RECURSE SRC "*.cpp")

add_library(resource_management ${SRC})

if (WIN32)
  file(GLOB LIBS "${CMAKE_SOURCE_DIR}/libs/slang/windows/*.lib")
elseif(APPLE)
  file(GLOB LIBS "${CMAKE_SOURCE_DIR}/libs/slang/macos/*.dylib")
elseif(UNIX)
  file(GLOB LIBS "${CMAKE_SOURCE_DIR}/libs/slang/linux/*.so")
endif()

target_link_libraries(resource_management ${LIBS})
