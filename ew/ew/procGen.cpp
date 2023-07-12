#include "procGen.h"

#include <stdlib.h>

namespace ew {
	static void createCubeFace(ew::Vec3 normal, float size, Mesh* mesh) {
		unsigned int startVertex = mesh->numVertices;
		ew::Vec3 a = ew::Vec3(normal.z, normal.x, normal.y); //U axis
		ew::Vec3 b = ew::Cross(normal, a); //V axis
		for (int i = 0; i < 4; i++)
		{
			int col = i % 2;
			int row = i / 2;

			ew::Vec3 pos = normal * size * 0.5f;
			//ew::Vec3 pos = ew::Vec3(0);
			pos -= (a + b) * size * 0.5f;
			pos += (a * col + b * row) * size;
			Vertex* vertex = &mesh->vertices[mesh->numVertices];
			vertex->pos = pos;
			vertex->uv = ew::Vec2(col, row);
			vertex->normal = normal;
			mesh->numVertices++;
		}

		//Indices
		mesh->indices[mesh->numIndices++] = startVertex;
		mesh->indices[mesh->numIndices++] = startVertex + 1;
		mesh->indices[mesh->numIndices++] = startVertex + 3;
		mesh->indices[mesh->numIndices++] = startVertex + 3;
		mesh->indices[mesh->numIndices++] = startVertex + 2;
		mesh->indices[mesh->numIndices++] = startVertex;
	}

	Mesh* createCubeMesh(float size) {
		float halfSize = size;
		Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
		if (mesh == NULL)
			return NULL;
		mesh->vertices = (Vertex*)malloc(sizeof(Vertex) * 24);
		if (mesh->vertices == NULL) {
			free(mesh);
			return NULL;
		}
		mesh->indices = (unsigned int*)malloc(sizeof(unsigned int) * 36);
		if (mesh->indices == NULL) {
			free(mesh);
			free(mesh->vertices);
			return NULL;
		}

		mesh->numVertices = 0;
		mesh->numIndices = 0;
		createCubeFace(ew::Vec3{ 0.0f,0.0f,1.0f }, size, mesh); //Front
		createCubeFace(ew::Vec3{ 1.0f,0.0f,0.0f }, size, mesh); //Right
		createCubeFace(ew::Vec3{ 0.0f,1.0f,0.0f }, size, mesh); //Top
		createCubeFace(ew::Vec3{ -1.0f,0.0f,0.0f }, size, mesh); //Left
		createCubeFace(ew::Vec3{ 0.0f,-1.0f,0.0f }, size, mesh); //Bottom
		createCubeFace(ew::Vec3{ 0.0f,0.0f,-1.0f }, size, mesh); //Back
		return mesh;
	}
}