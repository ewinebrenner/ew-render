#include "TextRenderer.h"
#include "external/glad.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

ew::TextRenderer::TextRenderer(ew::Font* font)
{
	m_font = font;
	glCreateVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	//Reserve enough bytes for character vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ew::TextRenderer::draw(const std::string& text, ew::Shader* shader, glm::vec4 color, float x, float y, float scale = 1.0)
{
	if (text.empty())
		return;

	shader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_font->m_textureHandle);
	shader->setInt("_FontTexture", 0);
	shader->setVec4("_Color", color);

	//TODO: Match to ref screen size
	glm::mat4 proj = glm::ortho(0.0f, 1080.0f, 0.0f, 720.0f);

	shader->setMat4("_Projection", proj);

	unsigned int numChars = text.size();

	std::vector<glm::vec4> vertexData;
	vertexData.reserve(numChars * 4);
	std::vector<unsigned int> indexData;
	indexData.reserve(numChars * 6);

	float startX = x;
	float startY = y;

	std::string::const_iterator c;
	for (c = text.begin(); c!=text.end();c++)
	{
		ew::Font::Character ch = m_font->m_characters[*c - 32];

		float xpos = startX + ch.bearing.x * scale;
		float ypos = startY - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		unsigned int startIndex = vertexData.size();

		vertexData.push_back(glm::vec4(xpos, ypos, ch.uvMin.x, ch.uvMin.y)); //Bottom Left
		vertexData.push_back(glm::vec4(xpos + w, ypos, ch.uvMax.x, ch.uvMin.y)); //Bottom right
		vertexData.push_back(glm::vec4(xpos + w, ypos + h, ch.uvMax.x, ch.uvMax.y)); //Top right
		vertexData.push_back(glm::vec4(xpos, ypos + h, ch.uvMin.x, ch.uvMax.y)); //Top left

		indexData.push_back(startIndex);
		indexData.push_back(startIndex+1);
		indexData.push_back(startIndex+2);
		indexData.push_back(startIndex+2);
		indexData.push_back(startIndex+3);
		indexData.push_back(startIndex);

		//ch.advance is in 1/64s of a pixel
		//bitshift by 6 to get value in pixels (2^6 = 64)
		startX += (ch.advance >> 6) * scale; 
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 4 * numChars, vertexData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * numChars, indexData.data(), GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, numChars*6, GL_UNSIGNED_INT, NULL);

	glDepthMask(GL_TRUE);
}
