#Camera example

file(
 GLOB_RECURSE A6_FLYCAMERA_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE A6_FLYCAMERA_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this assignment6_flyCamera's asset folder to bin when it is built
add_custom_target(copyAssetsFlyCamera ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${A6_FLYCAMERA_INC} DESTINATION include/assignment6_flyCamera)
add_executable(assignment6_flyCamera ${A6_FLYCAMERA_SRC} ${A6_FLYCAMERA_INC} ${A6_FLYCAMERA_ASSETS})
target_link_libraries(assignment6_flyCamera PUBLIC ew IMGUI)
target_include_directories(assignment6_flyCamera PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when assignment6_flyCamera is built
add_dependencies(assignment6_flyCamera copyAssetsFlyCamera)