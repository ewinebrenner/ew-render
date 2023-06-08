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

#Copies this helloTriangle's asset folder to bin when it is built
add_custom_target(copyAssets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${HELLOTRIANGLE_INC} DESTINATION include/helloTriangle)
add_executable(helloTriangle ${HELLOTRIANGLE_SRC} ${HELLOTRIANGLE_INC} ${HELLOTRIANGLE_ASSETS})
target_link_libraries(helloTriangle PUBLIC ew glew IMGUI glm assimp freetype)
target_include_directories(helloTriangle PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when helloTriangle is built
add_dependencies(helloTriangle copyAssets)