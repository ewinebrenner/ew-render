cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Include guards
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

project(EWRender)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/libs)

include(external/cpm.cmake)

# add libraries
include(external/glfw.cmake)
include(external/imgui.cmake)

add_subdirectory(ew)
add_subdirectory(examples/assignment1_helloTriangle)
add_subdirectory(examples/assignment2_sunset)
add_subdirectory(examples/assignment3_textures)
add_subdirectory(examples/assignment4_transformations)
add_subdirectory(examples/assignment5_camera)
add_subdirectory(examples/assignment6_flyCamera)
add_subdirectory(examples/assignment7_shapeGen)
add_subdirectory(examples/assignment8_lighting)
add_subdirectory(examples/example_spline)