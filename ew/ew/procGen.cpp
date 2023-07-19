#include "procGen.h"

#include <stdlib.h>

namespace ew {
	static void createCubeFace(ew::Vec3 normal, float size, MeshData* mesh) {
		unsigned int startVertex = mesh->vertices.size();
		ew::Vec3 a = ew::Vec3(normal.z, normal.x, normal.y); //U axis
		ew::Vec3 b = ew::Cross(normal, a); //V axis
		for (int i = 0; i < 4; i++)
		{
			int col = i % 2;
			int row = i / 2;

			ew::Vec3 pos = normal * size * 0.5f;
			pos -= (a + b) * size * 0.5f;
			pos += (a * col + b * row) * size;
			Vertex vertex;// = &mesh->vertices[mesh->vertices.size()];
			vertex.pos = pos;
			vertex.uv = ew::Vec2(col, row);
			vertex.normal = normal;
			mesh->vertices.push_back(vertex);
		}

		//Indices
		mesh->indices.push_back(startVertex);
		mesh->indices.push_back(startVertex + 1);
		mesh->indices.push_back(startVertex + 3);
		mesh->indices.push_back(startVertex + 3);
		mesh->indices.push_back(startVertex + 2);
		mesh->indices.push_back(startVertex);
	}

	 void createCube(float size, MeshData* mesh) {
		mesh->vertices.clear();
		mesh->vertices.reserve(24); //6 x 4 vertices
		mesh->indices.clear();
		mesh->indices.reserve(36); //6 x 6 indices
		float halfSize = size;
		createCubeFace(ew::Vec3{ 0.0f,0.0f,1.0f }, size, mesh); //Front
		createCubeFace(ew::Vec3{ 1.0f,0.0f,0.0f }, size, mesh); //Right
		createCubeFace(ew::Vec3{ 0.0f,1.0f,0.0f }, size, mesh); //Top
		createCubeFace(ew::Vec3{ -1.0f,0.0f,0.0f }, size, mesh); //Left
		createCubeFace(ew::Vec3{ 0.0f,-1.0f,0.0f }, size, mesh); //Bottom
		createCubeFace(ew::Vec3{ 0.0f,0.0f,-1.0f }, size, mesh); //Back
	 }
	 void createSphere(float radius, int numSegments, MeshData* mesh)
	 {
		 mesh->vertices.clear();
		 mesh->indices.clear();
		 //VERTICES---------------
		 //Top center
		 mesh->vertices.push_back({ Vec3(0.0f,radius,0.0f),Vec3(0,1,0) });
		 float thetaStep = (2 * PI) / numSegments; //Horizontal angle 
		 float phiStep = PI / numSegments; //Vertical angle
		 //Row
		 for (size_t i = 1; i < numSegments; i++)
		 {
			 float phi = i * phiStep;
			 //Column
			 for (size_t j = 0; j <= numSegments; j++)
			 {
				 float theta = j * thetaStep;
				 float x = cos(theta) * sin(phi);
				 float y = cos(phi);
				 float z = sin(theta) * sin(phi);
				 Vec3 pos = Vec3(x, y, z) * radius;
				 Vec3 normal = Vec3(x, y, z);
				 mesh->vertices.push_back({ pos,normal });
			 }
		 }
		 //Bottom
		 mesh->vertices.push_back({ Vec3(0.0f,-radius,0.0f),Vec3(0,-1,0) });
		 //INDICES-----------------
		 //Top cap
		 for (size_t i = 1; i <= numSegments; i++)
		 {
			 mesh->indices.push_back(i);
			 mesh->indices.push_back(0);
			 mesh->indices.push_back(i+1);
		 }
		 //Rows of quads
		 int start = 1; //Skip top vertex
		 int ringVertexCount = numSegments + 1;
		 for (size_t i = 0; i < numSegments - 2; i++)
		 {
			 for (size_t j = 0; j < numSegments; j++)
			 {
				 //Triangle 1
				 mesh->indices.push_back(start + i * ringVertexCount + j);
				 mesh->indices.push_back(start + i * ringVertexCount + j + 1);
				 mesh->indices.push_back(start + (i + 1) * ringVertexCount + j);
				 //Triangle 2
				 mesh->indices.push_back(start + (i + 1) * ringVertexCount + j);
				 mesh->indices.push_back(start + i * ringVertexCount + j + 1);
				 mesh->indices.push_back(start + (i+1) * ringVertexCount + j + 1);
			 }
		 }
		 //Bottom cap
		 unsigned int bottomIndex = mesh->vertices.size() - 1;
		 for (size_t i = 1; i <= numSegments + 1; i++)
		 {
			 mesh->indices.push_back(bottomIndex);
			 mesh->indices.push_back(bottomIndex - i);
			 mesh->indices.push_back(bottomIndex - i + 1);
		 }
	 }
}