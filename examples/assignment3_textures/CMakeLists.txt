#Hello Triangle example

file(
 GLOB_RECURSE A3_TEXTURES_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE A3_TEXTURES_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this assignment3_textures's asset folder to bin when it is built
add_custom_target(copyAssetsTextures ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${A3_TEXTURES_INC} DESTINATION include/assignment3_textures)
add_executable(assignment3_textures ${A3_TEXTURES_SRC} ${A3_TEXTURES_INC} ${A3_TEXTURES_ASSETS})
target_link_libraries(assignment3_textures PUBLIC ew IMGUI)
target_include_directories(assignment3_textures PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when assignment3_textures is built
add_dependencies(assignment3_textures copyAssetsTextures)