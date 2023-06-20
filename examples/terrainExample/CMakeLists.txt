#Hello Triangle example

file(
 GLOB_RECURSE TERRAIN_EXAMPLE_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE TERRAIN_EXAMPLE_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)

#Copies this helloTriangle's asset folder to bin when it is built
add_custom_target(copyAssetsTerrain ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${TERRAIN_EXAMPLE_INC} DESTINATION include/terrainExample)
add_executable(terrainExample ${TERRAIN_EXAMPLE_SRC} ${TERRAIN_EXAMPLE_INC} ${TERRAIN_EXAMPLE_ASSETS})
target_link_libraries(terrainExample PUBLIC ew IMGUI glm assimp)
target_include_directories(terrainExample PUBLIC ${EW_INC_DIR})
#Trigger asset copy when helloTriangle is built
add_dependencies(terrainExample copyAssetsTerrain)