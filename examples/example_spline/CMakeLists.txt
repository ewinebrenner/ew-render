#Camera example

file(
 GLOB_RECURSE EXAMPLE_SPLINE_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE EXAMPLE_SPLINE_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this example_spline's asset folder to bin when it is built
add_custom_target(copyAssets_example_spline ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

#Copies core asset folder to bin
add_custom_target(copyAssets_engineAssets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/../../assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/engine)

install(FILES ${EXAMPLE_SPLINE_INC} DESTINATION include/example_spline)
add_executable(example_spline ${EXAMPLE_SPLINE_SRC} ${EXAMPLE_SPLINE_INC} ${EXAMPLE_SPLINE_ASSETS})
target_link_libraries(example_spline PUBLIC ew IMGUI)
target_include_directories(example_spline PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when example_spline is built
add_dependencies(example_spline copyAssets_example_spline)
add_dependencies(example_spline copyAssets_engineAssets)