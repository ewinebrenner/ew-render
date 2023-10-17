#pragma once
#include "ewMath/vec4.h"

namespace ew {
	enum class TextureInternalFormat {
		RGB32UI,
		RGBF,
		RGBA32F,
		DEPTH
	};
	enum class TextureFormat {
		RGB_INTEGER,
		RGB,
		RGBA,
		DEPTH
	};
	enum class TextureType {
		FLOAT,
		UINT,
		UBYTE
	};
	struct PixelInfo {
		unsigned int r, g, b;
	};
	class Framebuffer {
	public:
		Framebuffer(unsigned int width, unsigned int height, TextureInternalFormat internalFormat, TextureFormat format, TextureType type);
		void resize(unsigned int width, unsigned int height);
		void bind();
		void unbind();
		inline unsigned int getWidth() { return m_width; }
		inline unsigned int getHeight() { return m_height; }
		PixelInfo readPixel(unsigned int x, unsigned int y);
	private:
		unsigned int m_id;
		unsigned int m_colorTexture;
		unsigned int m_depthTexture;
		unsigned int m_width, m_height;
		TextureInternalFormat m_internalFormat;
		TextureFormat m_format;
		TextureType m_type;
	};
}