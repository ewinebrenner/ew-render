#include "TextRenderer.h"
#include <GL/glew.h>
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

	//Reserve enough bytes for character vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ew::TextRenderer::draw(ew::Shader* shader, glm::vec4 color)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_font->m_textureHandle);
	shader->setInt("_FontTexture", 0);
	shader->setVec4("_Color", color);

	//TODO: Match to ref screen size
	glm::mat4 proj = glm::ortho(0.0f, 1080.0f, 0.0f, 720.0f);

	shader->setMat4("_Projection", proj);
	
	glBindVertexArray(m_vao);
	ew::Font::Character c = m_font->m_characters[0];


	std::vector<glm::vec4> vertexData;
	vertexData.reserve(6);

	float scale = 4;

	float xpos = 64.f;
	float ypos = 64.f;
	float w = c.size.x * scale;
	float h = c.size.y * scale;
	
	vertexData.push_back(glm::vec4(xpos, ypos, 0.0f, 1.0f));
	vertexData.push_back(glm::vec4(xpos + w, ypos, 1.0f, 1.0f));
	vertexData.push_back(glm::vec4(xpos + w, ypos + h, 1.0f, 0.0f));
	vertexData.push_back(glm::vec4(xpos + w, ypos + h, 1.0f, 0.0f));
	vertexData.push_back(glm::vec4(xpos, ypos+h, 0.0f, 0.0f));
	vertexData.push_back(glm::vec4(xpos, ypos, 0.0f, 1.0f));

	glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * 6, vertexData.data());

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
