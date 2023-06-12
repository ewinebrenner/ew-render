#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Texture.h"
#include "Shader.h"

namespace ew {
	class SpriteRenderer {
	public:
		SpriteRenderer();
		void begin(Shader* shader, const glm::mat4& transform);
		void draw(Texture* sourceTex, glm::vec4 sourceRect, glm::vec3 pos, glm::vec4 color, glm::vec2 scale, float rotation, glm::vec2 origin);
		void end();
	private:
		struct SpriteVertex {
			glm::vec3 position;
			int textureIndex;
			glm::vec4 color;
			glm::vec2 uv;
		};
		Shader* m_shader;
		std::vector<Texture*> m_textures;
		std::vector<SpriteVertex> m_vertices;
		std::vector<unsigned int> m_indices;
		unsigned int m_vao, m_vbo, m_ebo;
		glm::mat4 m_transform;
	};
}