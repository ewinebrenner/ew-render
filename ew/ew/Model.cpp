#include "Model.h"
#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool ew::Model::loadFromFile(const char* filePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate );
	if (scene == nullptr) {
		printf("Failed to load file %s", filePath);
		return false;
	}
	if (!scene->HasMeshes()) {
		printf("No meshes found in %s", filePath);
		return false;
	}

	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		m_meshes.push_back(convertMesh(scene->mMeshes[i]));
	}
	
	return true;
}

void ew::Model::draw() {
	size_t numMeshes = m_meshes.size();
	for (size_t i = 0; i < numMeshes; i++)
	{
		m_meshes[i].draw();
	}
}

ew::Mesh ew::Model::convertMesh(aiMesh* mesh) {
	ew::Mesh newMesh;

	//Gather vertices
	int numVertices = mesh->mNumVertices;
	std::vector<Vertex> vertices;
	vertices.reserve(numVertices);

	for (size_t i = 0; i < numVertices; i++)
	{
		aiVector3D aiPos = mesh->mVertices[i];

		Vertex v;
		v.position = glm::vec3(aiPos.x, aiPos.y, aiPos.z);
		if (mesh->HasNormals())
		{
			aiVector3D aiNormal = mesh->mNormals[i];
			v.normal = glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z);
		}
		if (mesh->HasTextureCoords(0)) {
			aiVector3D aiUV = mesh->mTextureCoords[0][i];
			v.uv = glm::vec2(aiUV.x, aiUV.y);
		}

		vertices.push_back(v);
	}

	//Gather indices
	int numIndices = 0;
	std::vector<unsigned int> indices;
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	newMesh.load(vertices, indices);
	return newMesh;
}