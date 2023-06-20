#include "Font.h"
#include "external/glad.h"
FT_Library ew::Font::m_ftLibrary = nullptr;
bool ew::Font::s_ftLibraryInitialized = false;

static bool s_ftLibraryInitialized;
ew::Font::Font(const char* filePath)
{
	if (!s_ftLibraryInitialized) {
		FT_Error error = FT_Init_FreeType(&m_ftLibrary);
		if (error){
			printf("ERROR::FREETYPE: Could not init FreeType Library");
			return;
		}
		s_ftLibraryInitialized = true;
	}

	FT_Face face;
	if (FT_New_Face(m_ftLibrary, filePath, 0, &face)) {
		printf("ERROR::FREETYPE: Failed to load font %s", filePath);
		return;
	}

	int glyphSize = 64;

	const int charSetLength = 95; //Asci 32-127
	int numColumns = 16;
	int numRows = charSetLength / numColumns + 1;

	int totalWidth = glyphSize * 1.1 * numColumns;
	int totalHeight = glyphSize * 1.2 * numRows;

	FT_Set_Pixel_Sizes(face, 0, glyphSize);

	//Create atlas texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &m_textureHandle);
	glBindTexture(GL_TEXTURE_2D, m_textureHandle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, totalWidth, totalHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	m_textureSize = glm::vec2(totalWidth, totalHeight);

	//write all characters to atlas texture
	for (int charIndex = 0; charIndex < charSetLength; charIndex++)
	{
		char c = charIndex + 32;
		
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			printf("Failed to load glph X");
		}
		
		FT_GlyphSlot slot = face->glyph;
		FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

		int col = charIndex % numColumns;
		int row = charIndex / numColumns;

		int xOffset = glyphSize * 1.1 * col;
		int yOffset = glyphSize * 1.2 * row;

		glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		Character chInfo;
		chInfo.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		chInfo.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		chInfo.advance = face->glyph->advance.x;

		glm::vec2 size = glm::vec2(face->glyph->bitmap.width / m_textureSize.x, face->glyph->bitmap.rows / m_textureSize.y);
		chInfo.uvMin = glm::vec2(xOffset / m_textureSize.x, (yOffset / m_textureSize.y));
		chInfo.uvMax = chInfo.uvMin + size;

		//Flip vertically
		float prevMin = chInfo.uvMin.y;
		chInfo.uvMin.y = chInfo.uvMax.y;
		chInfo.uvMax.y = prevMin;

		//chInfo.uvMin.y = 1.0 - chInfo.uvMin.y;
		//chInfo.uvMax.y = 1.0 - chInfo.uvMax.y;

		m_characters[charIndex] = chInfo;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	FT_Done_Face(face);
}

ew::Font::~Font()
{
	FT_Done_FreeType(m_ftLibrary);
}
