#pragma once

#include "Font.h"
#include "Shader.h"

namespace ew {
	class TextRenderer {
	public:
		TextRenderer(ew::Font* font);
		//TODO: Support writing arbitrary strings
		void draw(const std::string& text, ew::Shader* shader, glm::vec4 color);
	private:
		ew::Font* m_font;
		unsigned int m_vao, m_vbo;
	};
}