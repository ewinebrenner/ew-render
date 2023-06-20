#Hello Triangle example

file(
 GLOB_RECURSE TEXUREDQUAD_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE TEXUREDQUAD_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this texturedQuad's asset folder to bin when it is built
add_custom_target(copyAssetsTexturedQuad ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${TEXUREDQUAD_INC} DESTINATION include/texturedQuad)
add_executable(texturedQuad ${TEXUREDQUAD_SRC} ${TEXUREDQUAD_INC} ${TEXUREDQUAD_ASSETS})
target_link_libraries(texturedQuad PUBLIC ew IMGUI)
target_include_directories(texturedQuad PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when texturedQuad is built
add_dependencies(texturedQuad copyAssetsTexturedQuad)