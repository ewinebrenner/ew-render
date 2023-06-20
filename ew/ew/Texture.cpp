#include "Texture.h"
#include "external/stb_image.h"
#include "external/glad.h"
#include <assert.h>

ew::Texture::Texture(const char* filePath, bool mipmap)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, numChannels;
	unsigned char* data = stbi_load(filePath, &width, &height, &numChannels, 0);
	if (data == NULL) {
		printf("Failed to load texture %s", filePath);
		return;
	}
	assert(data != NULL);
	

	GLenum format = getOGLTextureFormat(numChannels);
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	//Configure Settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR));

	//Fill texture with data
	glTexImage2D(GL_TEXTURE_2D, 0, format,
		width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	m_width = width;
	m_height = height;
}

unsigned int ew::Texture::getOGLTextureFormat(int numChannels)
{
	switch (numChannels) {
	case 1:
		return GL_R16;
	case 2:
		return GL_RG;
	case 3:
		return GL_RGB;
	default:
		return GL_RGBA;
	}
}

void ew::Texture::setFilter(ew::FilterMode filterMode) {
	int glFilter;
	switch (filterMode) {
	case ew::FilterMode::Nearest:
		glFilter = GL_NEAREST;
		break;
	default:
		glFilter = GL_LINEAR;
		break;
	}
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
	glBindTexture(GL_TEXTURE_2D, 0);
	return;
}