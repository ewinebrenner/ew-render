#pragma once
#include <vector>
#include "mesh.h"
struct aiNode;
struct aiScene;
struct aiMesh;

namespace ew {
	class Model {
	public:
		Model(const MeshData& meshData);
		Model(const std::string& filePath);
		void Draw()const;
	private:
		std::vector<ew::Mesh> m_meshes;
		void processNode(aiNode* node, const aiScene* scene);
		ew::Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	};
}