cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(EWRender)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/libs)

include(external/cpm.cmake)

# add libraries
include(external/glfw.cmake)
include(external/glew.cmake)
include(external/imgui.cmake)
include(external/assimp.cmake)

# add executable
add_executable(demo src/main.cpp)

find_package(OpenGL REQUIRED)

target_link_libraries(demo PUBLIC glew IMGUI assimp)