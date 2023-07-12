#pragma once
#include <string>

namespace ew {
	std::string loadShaderSourceFromFile(const char* filePath);
	unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
}