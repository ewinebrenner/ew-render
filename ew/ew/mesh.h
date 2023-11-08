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
	inline void ResetBoneWeights(Vertex& vertex) {
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			vertex.boneIds[i] = -1;
			vertex.boneWeights[i] = 0.0f;
		}
	}
	inline void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
		{
			if (vertex.boneIds[i] < 0)
			{
				vertex.boneWeights[i] = weight;
				vertex.boneIds[i] = boneID;
				break;
			}
		}
	}
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