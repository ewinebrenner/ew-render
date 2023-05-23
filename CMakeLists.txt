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
include(external/glew.cmake)
include(external/imgui.cmake)
include(external/assimp.cmake)

# add executable
# gather all header (h,hpp) files for demo
file(
 GLOB
 DEMO_INCLUDE
 CONFIGURE_DEPENDS
 ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
 ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

# gather all source (c,cpp) files for demo
file(
 GLOB
 DEMO_SRC
 CONFIGURE_DEPENDS
 ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
 ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(demo ${DEMO_SRC} ${DEMO_INCLUDE})

find_package(OpenGL REQUIRED)

target_link_libraries(demo PUBLIC glew IMGUI assimp)