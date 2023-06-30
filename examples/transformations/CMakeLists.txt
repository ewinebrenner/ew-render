#Hello Triangle example

file(
 GLOB_RECURSE TRANSFORMATIONS_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE TRANSFORMATIONS_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this transformations's asset folder to bin when it is built
add_custom_target(copyAssetsTransformations ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${TRANSFORMATIONS_INC} DESTINATION include/transformations)
add_executable(transformations ${TRANSFORMATIONS_SRC} ${TRANSFORMATIONS_INC} ${TRANSFORMATIONS_ASSETS})
target_link_libraries(transformations PUBLIC ew IMGUI)
target_include_directories(transformations PUBLIC ${EW_INC_DIR} ${stb_INCLUDE_DIR})
#Trigger asset copy when transformations is built
add_dependencies(transformations copyAssetsTransformations)