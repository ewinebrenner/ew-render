#Hello Triangle example

file(
 GLOB_RECURSE HELLOTRIANGLE_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE HELLOTRIANGLE_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

install(FILES ${HELLOTRIANGLE_INC} DESTINATION include/helloTriangle)
add_executable(helloTriangle ${HELLOTRIANGLE_SRC} ${HELLOTRIANGLE_INC})
target_link_libraries(helloTriangle PUBLIC ew glew IMGUI glm assimp)
target_include_directories(helloTriangle PUBLIC ${EW_INC_DIR})