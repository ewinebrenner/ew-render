#pragma once
#include "ewMath/ewMath.h"

namespace ew {
	struct Vertex {
		ew::Vec3 pos;
		ew::Vec3 normal;
		ew::Vec2 uv;
	};

	//struct Mesh {
	//	void load(MeshData& meshData);
	//	unsigned int m_vao;
	//};

	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	unsigned int createVAO(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
}