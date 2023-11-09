#include "model.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assert.h>
#include "importUtil.h"

namespace ew {
    Model::Model(const MeshData& meshData)
    {
        ew::Mesh mesh;
        mesh.load(meshData);
        m_meshes.push_back(mesh);
    }
    Model::Model(const std::string& filePath)
	{
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            printf("ERROR::ASSIMP:: &s", import.GetErrorString());
            return;
        }
        processNode(scene->mRootNode, scene);
	}
    void Model::Draw()const
    {
        for (size_t i = 0; i < m_meshes.size(); i++)
        {
            m_meshes[i].Draw();
        }
    }
    void Model::processNode(aiNode* node, const aiScene* scene)
    {
        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(processMesh(mesh, scene));
        }
        for (size_t i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    void Model::ExtractBoneWeights(std::vector<Vertex>& vertices, aiMesh* aiMesh, const aiScene* scene) {
        for (size_t boneIndex = 0; boneIndex < aiMesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = aiMesh->mBones[boneIndex]->mName.C_Str();
            //Found a new bone name
            if (m_boneInfoMap.find(boneName) == m_boneInfoMap.end()) {
                BoneInfo newBoneInfo;
                newBoneInfo.id = m_boneCounter;
                newBoneInfo.offset = aiMat4(aiMesh->mBones[boneIndex]->mOffsetMatrix);
                m_boneInfoMap[boneName] = newBoneInfo;
                boneID = m_boneCounter;
                m_boneCounter++;
            }
            else {
                boneID = m_boneInfoMap[boneName].id;
            }
            assert(boneID != -1);

            //Assign weights for found bone
            aiVertexWeight* weights = aiMesh->mBones[boneIndex]->mWeights;
            int numWeights = aiMesh->mBones[boneIndex]->mNumWeights;
            for (size_t weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }
    ew::Mesh Model::processMesh(aiMesh* aiMesh, const aiScene* scene)
    {
        ew::MeshData meshData;
        for (size_t i = 0; i < aiMesh->mNumVertices; i++)
        {
            ew::Vertex vertex;
            ResetBoneWeights(vertex);
            vertex.pos = aiVec3(aiMesh->mVertices[i]);
            if (aiMesh->HasNormals()) {
                vertex.normal = aiVec3(aiMesh->mNormals[i]);
            }
            if (aiMesh->HasTangentsAndBitangents()) {
                vertex.tangent = aiVec3(aiMesh->mTangents[i]);
            }
            if (aiMesh->HasTextureCoords(0)) {
                vertex.uv = aiVec2(aiMesh->mTextureCoords[0][i]);
            }
            meshData.vertices.push_back(vertex);
        }
        for (size_t i = 0; i < aiMesh->mNumFaces; i++)
        {
            aiFace face = aiMesh->mFaces[i];
            for (size_t j = 0; j < face.mNumIndices; j++)
            {
                meshData.indices.push_back(face.mIndices[j]);
            }
        }
        ExtractBoneWeights(meshData.vertices, aiMesh, scene);
        ew::Mesh mesh;
        mesh.load(meshData);
        return mesh;
    }
}