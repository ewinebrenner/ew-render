#pragma once
#include "ewMath/ewMath.h"

namespace ew {
	struct Vertex {
		ew::Vec3 pos;
		ew::Vec3 normal;
		ew::Vec2 uv;
	};

	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	class Mesh {
	public:
		void load(const MeshData& meshData);
		void bind()const;
		inline int getNumVertices()const { return m_numVertices; }
		inline int getNumIndices()const { return m_numIndices; }
	private:
		bool m_initialized = false;
		unsigned int m_vao, m_vbo, m_ebo;
		int m_numVertices = 0, m_numIndices = 0;
	};

	unsigned int createVAO(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
}