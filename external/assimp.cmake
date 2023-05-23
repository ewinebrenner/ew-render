#ASSIMP
string(TIMESTAMP BEFORE "%s")

CPMAddPackage(
	NAME "assimp"
	URL "https://github.com/assimp/assimp/archive/refs/tags/v5.2.5.zip"
)
find_package(assimp REQUIRED)
set (assimp_INCLUDE_DIR ${assimp_SOURCE_DIR}/include)
include_directories(${assimp_INCLUDE_DIR})
string(TIMESTAMP AFTER "%s")
math(EXPR DELTAassimp "${AFTER}-${BEFORE}")
MESSAGE(STATUS "assimp TIME: ${DELTAassimp}s")
