#include "Font.h"
#include <GL/glew.h>
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
	FT_Set_Pixel_Sizes(face, 0, 64);

	//TODO: Load all characters
	if (FT_Load_Char(face, 'A', FT_LOAD_RENDER)) {
		printf("Failed to load glph X");
	}
	FT_GlyphSlot slot = face->glyph;
	FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

	//Create atlas texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &m_textureHandle);
	glBindTexture(GL_TEXTURE_2D, m_textureHandle);

	//TODO: write all characters to atlas texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Character c;
	c.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
	c.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
	c.advance = face->glyph->advance.x;
	m_characters[0] = c;

	FT_Done_Face(face);
}

ew::Font::~Font()
{
	FT_Done_FreeType(m_ftLibrary);
}
