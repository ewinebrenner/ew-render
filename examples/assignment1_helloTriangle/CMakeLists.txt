#Hello Triangle example

file(
 GLOB_RECURSE HELLOTRIANGLE_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE HELLOTRIANGLE_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this assignment1_helloTriangle's asset folder to bin when it is built
add_custom_target(copyAssetsA1 ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${HELLOTRIANGLE_INC} DESTINATION include/assignment1_helloTriangle)
add_executable(assignment1_helloTriangle ${HELLOTRIANGLE_SRC} ${HELLOTRIANGLE_INC} ${HELLOTRIANGLE_ASSETS})
target_link_libraries(assignment1_helloTriangle PUBLIC ew IMGUI)
target_include_directories(assignment1_helloTriangle PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when assignment1_helloTriangle is built
add_dependencies(assignment1_helloTriangle copyAssetsA1)