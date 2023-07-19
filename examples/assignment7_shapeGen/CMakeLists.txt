#Camera example

file(
 GLOB_RECURSE A7_SHAPEGEN_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE A7_SHAPEGEN_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this assignment7_shapeGen's asset folder to bin when it is built
add_custom_target(copyAssetsA7 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${A7_SHAPEGEN_INC} DESTINATION include/assignment7_shapeGen)
add_executable(assignment7_shapeGen ${A7_SHAPEGEN_SRC} ${A7_SHAPEGEN_INC} ${A7_SHAPEGEN_ASSETS})
target_link_libraries(assignment7_shapeGen PUBLIC ew IMGUI)
target_include_directories(assignment7_shapeGen PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when assignment7_shapeGen is built
add_dependencies(assignment7_shapeGen copyAssetsA7)