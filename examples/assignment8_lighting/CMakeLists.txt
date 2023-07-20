#Camera example

file(
 GLOB_RECURSE A8_LIGHTING_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE A8_LIGHTING_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this assignment8_lighting's asset folder to bin when it is built
add_custom_target(copyAssetsA8 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${A8_LIGHTING_INC} DESTINATION include/assignment8_lighting)
add_executable(assignment8_lighting ${A8_LIGHTING_SRC} ${A8_LIGHTING_INC} ${A8_LIGHTING_ASSETS})
target_link_libraries(assignment8_lighting PUBLIC ew IMGUI)
target_include_directories(assignment8_lighting PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when assignment8_lighting is built
add_dependencies(assignment8_lighting copyAssetsA8)