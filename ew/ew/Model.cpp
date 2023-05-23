#include "Model.h"
#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void ew::Model::init() {
	if (m_initialized)
		return;

	glCreateVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	//POSITION
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);

	//NORMAL
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);

	//UV
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, uv)));
	glEnableVertexAttribArray(2);

	//TANGENT
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, tangent)));
	glEnableVertexAttribArray(3);

	m_initialized = true;
}

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

	init();

	aiMesh* mesh = scene->mMeshes[0];

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

	m_vertices = vertices;
	m_indices = indices;

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	return true;
}

void ew::Model::draw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}
