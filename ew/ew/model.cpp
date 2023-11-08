#include "model.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

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
    ew::Vec3 aiVec3(const aiVector3D& v) {
        return ew::Vec3(v.x, v.y, v.z);
    }
    ew::Vec2 aiVec2(const aiVector3D& v) {
        return ew::Vec2(v.x, v.y);
    }
    ew::Mesh Model::processMesh(aiMesh* aiMesh, const aiScene* scene)
    {
        ew::MeshData meshData;
        for (size_t i = 0; i < aiMesh->mNumVertices; i++)
        {
            ew::Vertex vertex;
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
        ew::Mesh mesh;
        mesh.load(meshData);
        return mesh;
    }
}