#pragma once
#include "ewMath/ewMath.h"

namespace ew {
#define MAX_BONE_INFLUENCE 4
	struct Vertex {
		ew::Vec3 pos;
		ew::Vec3 normal;
		ew::Vec2 uv;
		ew::Vec3 tangent;
		int boneIds[MAX_BONE_INFLUENCE];
		float boneWeights[MAX_BONE_INFLUENCE];
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
		void Draw()const;
	private:
		bool m_initialized = false;
		unsigned int m_vao = 0;
		unsigned int m_vbo = 0;
		unsigned int m_ebo = 0;
		int m_numVertices = 0;
		int m_numIndices = 0;
	};
}