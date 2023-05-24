#pragma once
#include <vector>
#include "Vertex.h"

namespace ew {
	class Mesh {
	public:
		Mesh();
		void load(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
		void draw();
	private:
		bool m_initialized = false;
		unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;
		std::vector<Vertex> m_vertices;
		std::vector<unsigned int> m_indices;
	};
}