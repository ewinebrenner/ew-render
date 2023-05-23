#pragma once
#include <glm/glm.hpp>

#include <vector>

namespace ew {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 tangent;
	};
	class Model {
	public:
		Model() {};
		bool loadFromFile(const char* filePath);
		void draw();
	private:
		unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;
		bool m_initialized = false;
		void init();
		std::vector<Vertex> m_vertices;
		std::vector<unsigned int> m_indices;
	};
}
