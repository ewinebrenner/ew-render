#include "external/glad.h"
#include "SpriteRenderer.h"

ew::SpriteRenderer::SpriteRenderer()
{
	glCreateVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	//Position (3 floats)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(SpriteVertex), 0);
	//Texture Index (1 int)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, sizeof(SpriteVertex), (const void*)offsetof(SpriteVertex,textureIndex));
	//Color (4 floats)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (const void*)offsetof(SpriteVertex,color));
	//UV (2 floats)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (const void*)offsetof(SpriteVertex,uv));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ew::SpriteRenderer::begin(Shader* shader, const glm::mat4& transform)
{
	m_vertices.clear();
	m_indices.clear();
	m_textures.clear();
	m_shader = shader;
	m_transform = transform;
}

void ew::SpriteRenderer::draw(Texture* sourceTex, glm::vec4 sourceRect, glm::vec3 pos, glm::vec4 color, glm::vec2 scale, float rotation, glm::vec2 origin)
{
	//Zero area source rect, nothing will be visible
	if (sourceRect.z <= 0 || sourceRect.w <= 0)
		return;

	//Find or add texture to textures array
	//Texture index is used to choose from uniform sampler2D _Textures[]
	int textureIndex;
	std::vector<Texture*>::iterator it = std::find(m_textures.begin(), m_textures.end(), sourceTex);
	if (it == m_textures.end()) {
		m_textures.push_back(sourceTex);
		textureIndex = m_textures.size() - 1;
	}
	else {
		textureIndex = it - m_textures.begin();
	}

	//Vertex positions
	//Pre-combined TRS matrix
	float rad = glm::radians(rotation);
	glm::mat3 transform = glm::mat3(1);
	transform[0][0] = glm::cos(rad) * scale.x;
	transform[1][0] = -glm::sin(rad) * scale.y;
	transform[0][1] = glm::sin(rad) * scale.x;
	transform[1][1] = glm::cos(rad) * scale.y;
	transform[2][0] = pos.x;
	transform[2][1] = pos.y;
	transform[2][2] = pos.z;

	glm::vec3 vOrigin = glm::vec3(-origin.x, -origin.y, 1.0f);

	glm::vec3 vPositions[4] = {
		 transform * vOrigin, //Bottom left
		 transform * (vOrigin + glm::vec3(1.0f,0.0f,0.0f)), //Bottom right
		 transform * (vOrigin + glm::vec3(1.0f,1.0f,0.0f)), //Top right
		 transform * (vOrigin + glm::vec3(0.0f,1.0f,0.0f)) //Top left
	};

	//Calculate 0-1 UVs for each quad
	float texWidth = sourceTex->getWidth();
	float texHeight = sourceTex->getHeight();
	float uvWidth = sourceRect.z / texWidth;
	float uvHeight = sourceRect.w / texHeight;
	glm::vec2 uvMin = glm::vec2(sourceRect.x / texWidth, sourceRect.y / texHeight);
	glm::vec2 vUVs[4] = {
		uvMin,
		uvMin + glm::vec2(uvWidth, 0.0f),
		uvMin + glm::vec2(uvWidth,uvHeight),
		uvMin + glm::vec2(0.0f, uvHeight)
	};

	unsigned int startIndex = m_vertices.size();
	for (size_t i = 0; i < 4; i++)
	{
		SpriteVertex v;
		v.position = vPositions[i];
		v.textureIndex = (float)textureIndex;
		v.color = color;
		v.uv = vUVs[i];
		m_vertices.push_back(v);
	}

	//2 triangles, CCW winding order
	m_indices.push_back(startIndex);
	m_indices.push_back(startIndex + 1);
	m_indices.push_back(startIndex + 2);
	m_indices.push_back(startIndex + 2);
	m_indices.push_back(startIndex + 3);
	m_indices.push_back(startIndex);
}

/// <summary>
/// Draws to the screen
/// </summary>
void ew::SpriteRenderer::end()
{
	m_shader->use();

	//Bind all textures used
	size_t numTextures = m_textures.size();
	for (size_t i = 0; i < numTextures; i++)
	{
		glActiveTexture(GL_TEXTURE0 + (unsigned int)i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]->getHandle());
		m_shader->setInt("_Textures[" + std::to_string(i) + "]", (int)i);
	}
	m_shader->setMat4("_MVP", m_transform);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteVertex) * m_vertices.size(), m_vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indices.size(), m_indices.data(), GL_DYNAMIC_DRAW);

	glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}
