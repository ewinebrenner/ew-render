#Camera example

file(
 GLOB_RECURSE CAMERA_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE CAMERA_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this camera's asset folder to bin when it is built
add_custom_target(copyAssetsCamera ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${CAMERA_INC} DESTINATION include/camera)
add_executable(camera ${CAMERA_SRC} ${CAMERA_INC} ${CAMERA_ASSETS})
target_link_libraries(camera PUBLIC ew IMGUI)
target_include_directories(camera PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when camera is built
add_dependencies(camera copyAssetsCamera)