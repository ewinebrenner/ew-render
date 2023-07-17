#Hello Triangle example

file(
 GLOB_RECURSE A2_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE A2_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this assignment2_sunset's asset folder to bin when it is built
add_custom_target(copyAssetsA2 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${A2_INC} DESTINATION include/assignment2_sunset)
add_executable(assignment2_sunset ${A2_SRC} ${A2_INC} ${A2_ASSETS})
target_link_libraries(assignment2_sunset PUBLIC ew IMGUI)
target_include_directories(assignment2_sunset PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when assignment2_sunset is built
add_dependencies(assignment2_sunset copyAssetsA2)