#pragma once

namespace ew {
	class Texture {
	public:
		Texture(const char* filePath, bool mipmap = true);
	private:
		unsigned int m_id = 0;
		unsigned int getOGLTextureFormat(int numChannels);
		inline unsigned int getHandle()const { return m_id; }
		friend class Material;
	};
}