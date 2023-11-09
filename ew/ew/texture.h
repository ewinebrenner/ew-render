#pragma once
#include "external/glad.h"

namespace ew {
	//TODO: Add option for vertical flip
	unsigned int loadTexture(const char* filePath, int wrapMode, int minFilter = GL_LINEAR_MIPMAP_LINEAR, int magFilter = GL_LINEAR);
}
