#pragma once
#include <vector>
#include <map>
#include "mesh.h"
struct aiNode;
struct aiScene;
struct aiMesh;

namespace ew {
	struct BoneInfo {
		int id;
		ew::Mat4 offset;
	};
	class Model {
	public:
		Model(const MeshData& meshData);
		Model(const std::string& filePath);
		void Draw()const;
	private:
		std::vector<ew::Mesh> m_meshes;
		void processNode(aiNode* node, const aiScene* scene);
		ew::Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		void ExtractBoneWeights(std::vector<Vertex>& vertices, aiMesh* aiMesh, const aiScene* scene);

		//Skeletal mesh
		std::map<std::string, BoneInfo> m_boneInfoMap;
		int m_boneCounter = 0;

	};
}