#Text example

file(
 GLOB_RECURSE TEXTEXAMPLE_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE TEXTEXAMPLE_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this textExample's asset folder to bin when it is built
add_custom_target(textExampleCopyAssets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${TEXTEXAMPLE_INC} DESTINATION include/textExample)
add_executable(textExample ${TEXTEXAMPLE_SRC} ${TEXTEXAMPLE_INC} ${TEXTEXAMPLE_ASSETS})
target_link_libraries(textExample PUBLIC ew IMGUI glm assimp freetype)
target_include_directories(textExample PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when textExample is built
add_dependencies(textExample textExampleCopyAssets)