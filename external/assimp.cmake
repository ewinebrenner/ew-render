#ASSIMP
string(TIMESTAMP BEFORE "%s")

CPMAddPackage(
	NAME "assimp"
	URL "https://github.com/assimp/assimp/archive/refs/tags/v5.2.5.zip"
	OPTIONS ("ASSIMP_BUILD_SAMPLES OFF" "ASSIMP_BUILD_ZLIB ON" "BUILD_SHARED_LIBS OFF")
)

find_package(assimp REQUIRED)
set (assimp_INCLUDE_DIR ${assimp_SOURCE_DIR}/include)
include_directories(${assimp_INCLUDE_DIR})
string(TIMESTAMP AFTER "%s")
math(EXPR DELTAassimp "${AFTER}-${BEFORE}")
MESSAGE(STATUS "assimp TIME: ${DELTAassimp}s")
