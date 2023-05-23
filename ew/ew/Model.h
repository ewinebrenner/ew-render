#pragma once
#include <glm/glm.hpp>

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
	private:
		unsigned int m_vao, m_vbo, m_ebo;
		bool m_initialized;
		void init();
	};
}
