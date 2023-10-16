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
		 mesh->vertices.push_back({ Vec3(0.0f,radius,0.0f),Vec3(0,1,0),Vec2(0.5,0.5)});
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
				 Vertex vertex;
				 vertex.pos = Vec3(x, y, z) * radius;
				 vertex.normal = Vec3(x, y, z);
				 vertex.uv = Vec2((float)j / numSegments, 1.0 - (float)i / numSegments);
				 mesh->vertices.push_back(vertex);
			 }
		 }
		 //Bottom
		 mesh->vertices.push_back({ Vec3(0.0f,-radius,0.0f),Vec3(0,-1,0),Vec2(0.5,0.5)});
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
	 void createRingVertices(float y, float radius, int numSegments, bool isCap, MeshData* mesh) {
		 float thetaStep = (2 * PI) / numSegments; //Horizontal angle 
		 for (size_t i = 0; i <= numSegments; i++)
		 {
			 float theta = i * thetaStep;
			 float x = cos(theta);
			 float z = sin(theta);
			 Vertex vertex;
			 vertex.pos = Vec3(x * radius, y, z * radius);
			 vertex.normal = isCap ? Normalize(Vec3(0,y,0)) : Normalize(Vec3(x, 0, z));
			 vertex.uv = isCap ? Vec2(x * 0.5 + 0.5, z * 0.5 + 0.5) : Vec2((float)i / numSegments, y > 0 ? 1 : 0);
			 mesh->vertices.push_back(vertex);
		 }
	 }
	 void createCylinder(float height, float radius, int numSegments, MeshData* mesh)
	 {
		 mesh->vertices.clear();
		 mesh->indices.clear();
		 float halfHeight = height / 2.0f;
		 //VERTICES---------------
		 //Top center
		 mesh->vertices.push_back({ Vec3(0.0f,halfHeight,0.0f),Vec3(0,1,0),Vec2(0.5)});
		 //Top ring (facing up)
		 createRingVertices(halfHeight, radius, numSegments, true, mesh);
		 unsigned int sideStart = mesh->vertices.size() - 1;
		 //Top ring (facing side)
		 createRingVertices(halfHeight, radius, numSegments, false, mesh);
		 //Bottom ring (facing side)
		 createRingVertices(-halfHeight, radius, numSegments, false, mesh);
		 //Bottom ring (facing down)
		 createRingVertices(-halfHeight, radius, numSegments, true, mesh);
		 //Bottom center
		 mesh->vertices.push_back({ Vec3(0.0f,-halfHeight,0.0f),Vec3(0,-1,0),Vec2(0.5)});
		 //INDICES-----------
		 //Top cap
		 for (size_t i = 1; i <= numSegments; i++)
		 {
			 mesh->indices.push_back(i);
			 mesh->indices.push_back(0);
			 mesh->indices.push_back(i + 1);
		 }
		 //Side quads
		 int ringVertexCount = numSegments + 1;
		 for (size_t i = 0; i <= numSegments; i++)
		 {
			 int start = sideStart + i; //Starting index of quad
			 mesh->indices.push_back(start);
			 mesh->indices.push_back(start + 1);
			 mesh->indices.push_back(start + ringVertexCount);
			 mesh->indices.push_back(start + ringVertexCount);
			 mesh->indices.push_back(start + 1);
			 mesh->indices.push_back(start + ringVertexCount+1);
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
	 void createPlane(float size, int numSegments, MeshData* mesh)
	 {
		 mesh->vertices.clear();
		 mesh->indices.clear();
		 //VERTICES
		 for (size_t i = 0; i <= numSegments; i++)
		 {
			 for (size_t j = 0; j <= numSegments; j++)
			 {
				 float u = (float)i / numSegments;
				 float v = (float)j / numSegments;
				 Vertex vertex;
				 vertex.pos = Vec3(-size/2 + size * u, 0.0f, -(-size / 2 + size * v));
				 vertex.normal = Vec3(0.0f, 1.0f, 0.0f);
				 vertex.uv = Vec2(u, v);
				 mesh->vertices.push_back(vertex);
			 }
		 }
		 //INDICES
		 int numColumns = numSegments + 1;
		 for (size_t i = 0; i < numSegments; i++)
		 {
			 for (size_t j = 0; j < numSegments; j++)
			 {
				 int start = i + j * numColumns;
				 //Triangle 1
				 mesh->indices.push_back(start);
				 mesh->indices.push_back(start + numColumns);
				 mesh->indices.push_back(start + numColumns + 1);
				 mesh->indices.push_back(start + numColumns + 1);
				 mesh->indices.push_back(start + 1);
				 mesh->indices.push_back(start);
			 }
		 }
	 }
	 void createTorus(float innerRadius, float outerRadius, int numRings, int numRingSegments, MeshData* meshData)
	 {
		 meshData->vertices.clear();
		 meshData->indices.clear();
		 float phiStep = (ew::PI * 2) / numRingSegments;
		 float thetaStep = (ew::PI * 2) / numRings;
		 for (size_t i = 0; i < numRings; i++)
		 {
			 float theta = thetaStep * i;
			 for (size_t j = 0; j < numRingSegments; j++)
			 {
				 float phi = phiStep * j;
				 Vertex v;
				 v.pos.x = cos(theta) * (outerRadius + cos(phi) * innerRadius);
				 v.pos.y = sin(theta) * (outerRadius + cos(phi) * innerRadius);
				 v.pos.z = sin(phi) * innerRadius;
				 v.normal = ew::Vec3(1);
				 v.uv = ew::Vec2(1);
				 meshData->vertices.push_back(v);
			 }
		 }
	 }
}