#Camera example

file(
 GLOB_RECURSE A5_CAMERA_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE A5_CAMERA_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this assignment5_camera's asset folder to bin when it is built
add_custom_target(copyAssetsCamera ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${A5_CAMERA_INC} DESTINATION include/assignment5_camera)
add_executable(assignment5_camera ${A5_CAMERA_SRC} ${A5_CAMERA_INC} ${A5_CAMERA_ASSETS})
target_link_libraries(assignment5_camera PUBLIC ew IMGUI)
target_include_directories(assignment5_camera PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when assignment5_camera is built
add_dependencies(assignment5_camera copyAssetsCamera)