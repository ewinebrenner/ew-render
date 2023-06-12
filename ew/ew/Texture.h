#pragma once

namespace ew {
	enum class FilterMode {
		Nearest,
		Bilinear
	};

	class Texture {
	public:
		Texture(const char* filePath, bool mipmap = true);
		void setFilter(ew::FilterMode filterMode);
		inline unsigned int getWidth()const { return m_width; }
		inline unsigned int getHeight()const { return m_height; }
	private:
		unsigned int m_width, m_height;
		unsigned int m_id = 0;
		unsigned int getOGLTextureFormat(int numChannels);
		inline unsigned int getHandle()const { return m_id; }
		friend class Material;
		friend class SpriteRenderer;
	};
}