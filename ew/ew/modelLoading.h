#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "mesh.h"

namespace ew {
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices)
	void parse_single_bone(int bone_index, const aiBone* pBone) {
		printf("Bone %d: '%s' num vertices affected by this bone: %d\n", bone_index, pBone->mName.C_Str(), pBone->mNumWeights);
		for (size_t i = 0; i < pBone->mNumWeights; i++)
		{
			if (i == 0)
				printf("\n");
			const aiVertexWeight& vw = pBone->mWeights[i];
			printf("        %d: vertex id %d weight %.2f\n", i, (int)vw.mVertexId, vw.mWeight);
		}
		printf("\n");
	}
	void parse_mesh_bones(const aiMesh* pMesh) {
		for (size_t i = 0; i < pMesh->mNumBones; i++)
		{
			parse_single_bone(i,pMesh->mBones[i]);
		}
	}
	void parse_meshes(const aiScene* pScene) {
		printf("Found %d meshes", pScene->mNumMeshes);
		for (size_t i = 0; i < pScene->mNumMeshes; i++)
		{
			const aiMesh* pMesh = pScene->mMeshes[i];
			printf("Num vertices: %d\n", pMesh->mNumVertices);
			printf("Num indices: %d\n", pMesh->mNumFaces * 3);
			printf("Num bones: %d\n", pMesh->mNumBones);
			if (pMesh->HasBones()) {
				parse_mesh_bones(pMesh);
			}
		}
	}

	int read_model(const char* filePath) {
		Assimp::Importer importer;
		const aiScene* pScene = importer.ReadFile(filePath, ASSIMP_LOAD_FLAGS);
		if (!pScene) {
			printf("Failed to load file %s", filePath);
			return 1;
		}
		parse_meshes(pScene);
		return 0;
	}
}