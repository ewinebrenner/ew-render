#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "glm/glm.hpp"

namespace ew {
	class Font {
	public:
		Font(const char* filePath);
		~Font();
	private:
		unsigned int m_textureHandle = 0;

		static FT_Library m_ftLibrary;
		static bool s_ftLibraryInitialized;
		struct Character {
			glm::ivec2 size; //Size of glyph in pixels
			glm::ivec2 bearing; //Offset from baseline to left/top of glyph
			unsigned int advance; // Offset to advance to next glyph
		};
		Character m_characters[128];
		friend class TextRenderer;
	};
}