#pragma once

namespace ew {
	class Texture {
	public:
		Texture(const char* filePath);
		inline unsigned int getHandle()const { return m_id; }
	private:
		unsigned int m_id = 0;
		unsigned int getOGLTextureFormat(int numChannels);
	};
}