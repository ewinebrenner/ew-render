#GLM
string(TIMESTAMP BEFORE "%s")

CPMAddPackage(
	NAME "glm"
	URL "https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip"
)
find_package(glm REQUIRED)
set (glm_INCLUDE_DIR ${glm_SOURCE_DIR}/include)
include_directories(${glm_INCLUDE_DIR})
string(TIMESTAMP AFTER "%s")
math(EXPR DELTAglm "${AFTER}-${BEFORE}")
MESSAGE(STATUS "glm TIME: ${DELTAglm}s")
