#include "framebuffer.h"
#include "external/glad.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

namespace ew {
	GLint getGLInternalFormat(TextureInternalFormat format) {
		switch (format) {
		case TextureInternalFormat::DEPTH:
			return GL_DEPTH;
		case TextureInternalFormat::RGB32UI:
			return GL_RGB32UI;
		case TextureInternalFormat::RGBA32F:
		default:
			return GL_RGBA32F;
		}
	}
	GLint getGLFormat(TextureFormat format) {
		switch (format) {
		case TextureFormat::RGB:
			return GL_RGB;
		case TextureFormat::RGB_INTEGER:
			return GL_RGB_INTEGER;
		case TextureFormat::DEPTH:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::RGBA:
		default:
			return GL_RGBA;
		}
	}
	GLint getGLType(TextureType type) {
		switch (type) {
		case TextureType::FLOAT:
			return GL_FLOAT;
		case TextureType::UINT:
			return GL_UNSIGNED_INT;
		case TextureType::UBYTE:
		default:
			return GL_UNSIGNED_BYTE;
		}
	}
	Framebuffer::Framebuffer(unsigned int width, unsigned int height, TextureInternalFormat internalFormat, TextureFormat format, TextureType type)
		:m_width(width), m_height(height), m_internalFormat(internalFormat), m_format(format), m_type(type)
	{
		assert(width > 0 && height > 0);

		glGenFramebuffers(1, &m_id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);

		//1 color attachment
		glGenTextures(1, &m_colorTexture);
		glBindTexture(GL_TEXTURE_2D, m_colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, getGLInternalFormat(internalFormat), width, height, 0, getGLFormat(format), getGLType(type), NULL);
		//TODO: Add support for changing filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

		//Depth buffer
		glGenTextures(1, &m_depthTexture);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status:%x\n", status);
			exit(1);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	}
	void Framebuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	PixelInfo Framebuffer::readPixel(unsigned int x, unsigned int y)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		PixelInfo pixelInfo;
		glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixelInfo);
		glReadBuffer(GL_NONE);
		glBindBuffer(GL_READ_FRAMEBUFFER, 0);
		return pixelInfo;
	}
	void Framebuffer::resize(unsigned int width, unsigned int height) {
		m_width = width;
		m_height = height;
		glBindTexture(GL_TEXTURE_2D, m_colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, getGLInternalFormat(m_internalFormat), m_width, m_height, 0, getGLFormat(m_format), getGLType(m_type), NULL);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}