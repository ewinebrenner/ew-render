#pragma once
#include "ewMath/ewMath.h"

namespace ew {
	struct Vertex {
		ew::Vec3 pos;
		ew::Vec3 normal;
		ew::Vec2 uv;
	};

	struct Mesh {
		Vertex* vertices;
		unsigned int numVertices;
		unsigned int* indices;
		unsigned int numIndices;
	};

	unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
}