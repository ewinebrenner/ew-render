#include "procGen.h"

#include <stdlib.h>
#include <assert.h>

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
	 void createSphereOld(float radius, int numSegments, MeshData* mesh)
	 {
		 mesh->vertices.clear();
		 mesh->indices.clear();
		 //VERTICES---------------
		 //Top center
		 mesh->vertices.push_back({ Vec3(0.0f,radius,0.0f),Vec3(0,1,0),Vec2(0.5,0.5)});
		 float thetaStep = (2 * PI) / numSegments; //Horizontal angle 
		 float phiStep = PI / numSegments; //Vertical angle
		 //Row
		 for (size_t row = 1; row < numSegments; row++)
		 {
			 float phi = row * phiStep;
			 //Column
			 for (size_t col = 0; col <= numSegments; col++)
			 {
				 float theta = col * thetaStep;
				 float x = cos(theta) * sin(phi);
				 float y = cos(phi);
				 float z = sin(theta) * sin(phi);
				 Vertex vertex;
				 vertex.pos = Vec3(x, y, z) * radius;
				 vertex.normal = Vec3(x, y, z);
				 vertex.uv = Vec2((float)col / numSegments, 1.0 - (float)row / numSegments);
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
		 int sideStart = 1; //Starting index of first row
		 int columns = numSegments + 1;
		 for (size_t row = 0; row < numSegments - 2; row++)
		 {
			 for (size_t col = 0; col < numSegments; col++)
			 {
				 int start = sideStart + row * columns + col;
				 //Triangle 1
				 mesh->indices.push_back(start);
				 mesh->indices.push_back(start + 1);
				 mesh->indices.push_back(start + columns);
				 //Triangle 2 
				 mesh->indices.push_back(start + columns);
				 mesh->indices.push_back(start + 1);
				 mesh->indices.push_back(start + columns + 1);
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
	 void createSphere(float radius, int numSegments, MeshData* mesh)
	 {
		 mesh->vertices.clear();
		 mesh->indices.clear();

		 float thetaStep = (2 * PI) / numSegments; //Horizontal angle 
		 float phiStep = PI / numSegments; //Vertical angle

		 //VERTICES---------------
		 //Row
		 for (size_t row = 0; row <= numSegments; row++)
		 {
			 float phi = row * phiStep;
			 //Column
			 for (size_t col = 0; col <= numSegments; col++)
			 {
				 float theta = col * thetaStep;
				 float x = cos(theta) * sin(phi);
				 float y = cos(phi);
				 float z = sin(theta) * sin(phi);
				 Vertex vertex;
				 vertex.pos = Vec3(x, y, z) * radius;
				 vertex.normal = Vec3(x, y, z);
				 vertex.uv = Vec2((float)col / numSegments, 1.0 - (float)row / numSegments);
				 mesh->vertices.push_back(vertex);
			 }
		 }

		 //INDICES-----------------
		 //Top cap
		 int topSidesStart = numSegments + 1;
		 for (size_t i = 0; i < numSegments; i++)
		 {
			 mesh->indices.push_back(topSidesStart + i);
			 mesh->indices.push_back(i); //Center
			 mesh->indices.push_back(topSidesStart + i + 1);
		 }
		 
		 //Rows of quads
		 int columns = numSegments + 1;
		 for (size_t row = 1; row < numSegments - 1; row++)
		 {
			 for (size_t col = 0; col < numSegments; col++)
			 {
				 int start = row * columns + col;
				 //Triangle 1
				 mesh->indices.push_back(start);
				 mesh->indices.push_back(start + 1);
				 mesh->indices.push_back(start + columns);
				 //Triangle 2 
				 mesh->indices.push_back(start + columns);
				 mesh->indices.push_back(start + 1);
				 mesh->indices.push_back(start + columns + 1);
			 }
		 }
		 //Bottom cap
		 int bottomCenterStart = mesh->vertices.size() - numSegments - 2;
		 int bottomRowStart = bottomCenterStart - numSegments - 1;
		 for (size_t i = 0; i <= numSegments; i++)
		 {
			 mesh->indices.push_back(bottomRowStart + i);
			 mesh->indices.push_back(bottomRowStart + i + 1); 
			 mesh->indices.push_back(bottomCenterStart + i); //Center
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
		 int start = 1;
		 int center = 0;
		 for (size_t i = 0; i <= numSegments; i++)
		 {
			 mesh->indices.push_back(start + i);
			 mesh->indices.push_back(center);
			 mesh->indices.push_back(start + i + 1);
		 }
		 //Side quads
		 int numColumns = numSegments + 1;
		 for (size_t i = 0; i <= numSegments; i++)
		 {
			 int start = sideStart + i; //Starting index of quad
			 mesh->indices.push_back(start);
			 mesh->indices.push_back(start + 1);
			 mesh->indices.push_back(start + numColumns);
			 mesh->indices.push_back(start + numColumns);
			 mesh->indices.push_back(start + 1);
			 mesh->indices.push_back(start + numColumns+1);
		 }
		 //Bottom cap
		 center = mesh->vertices.size() - 1;
		 for (size_t i = 1; i <= numSegments + 1; i++)
		 {
			 mesh->indices.push_back(center);
			 mesh->indices.push_back(center - i);
			 mesh->indices.push_back(center - i + 1);
		 }
	 }
	 void createCone(float height, float radius, int numSegments, int numStacks, float stackCurve, MeshData* meshData)
	 {
		 assert(numSegments > 2 && numStacks > 0);
		 meshData->vertices.clear();
		 meshData->indices.clear();
		 float thetaStep = (2 * PI) / numSegments; //Horizontal angle 

		 float phi = atan(height / radius);
		 phi += PI / 2;

		 //i == 0 BOTTOM i == 1 TOP
		 for (size_t i = 0; i <= numStacks; i++)
		 {
			 float stackT = (float)i / numStacks;
			 float stackTAdjusted = pow(stackT, stackCurve);
			 for (size_t j = 0; j <= numSegments; j++)
			 {
				 float theta = j * thetaStep;
				 float cosT = cos(theta);
				 float sinT = sin(theta);

				 Vertex vertex;
				 float r = radius * (1 - stackTAdjusted);
				 vertex.pos = Vec3(cosT * r, -height / 2 + height * stackTAdjusted, sinT * r);
				 vertex.normal = Vec3(-cosT * cos(phi), sin(phi),-sinT * cos(phi));
				 vertex.uv = Vec2((float)j/numSegments, stackTAdjusted);
				 meshData->vertices.push_back(vertex);
			 }
		 }

		 //Side quads
		 int columns = numSegments + 1;
		 for (size_t i = 0; i < numStacks; i++)
		 {
			 for (size_t j = 0; j <= numSegments; j++)
			 {
				 int start = j + i * columns;
				 meshData->indices.push_back(start);
				 meshData->indices.push_back(start + columns);
				 meshData->indices.push_back(start + 1);
				 meshData->indices.push_back(start + 1);
				 meshData->indices.push_back(start + columns);
				 meshData->indices.push_back(start + columns + 1);
			 }
		 }
		 
	 }

	 void createPlane(float size, int subdivisions, MeshData* mesh)
	 {
		 mesh->vertices.clear();
		 mesh->indices.clear();
		 //VERTICES
		 for (size_t row = 0; row <= subdivisions; row++)
		 {
			 for (size_t col = 0; col <= subdivisions; col++)
			 {
				 float u = (float)col / subdivisions;
				 float v = (float)row / subdivisions;
				 Vertex vertex;
				 vertex.pos = Vec3(-size/2 + size * u, 0.0f, -(-size / 2 + size * v));
				 vertex.normal = Vec3(0.0f, 1.0f, 0.0f);
				 vertex.uv = Vec2(u, v);
				 mesh->vertices.push_back(vertex);
			 }
		 }
		 //INDICES
		 int numColumns = subdivisions + 1;
		 for (size_t row = 0; row < subdivisions; row++)
		 {
			 for (size_t col = 0; col < subdivisions; col++)
			 {
				 int start = row + col * numColumns;
				 //Triangle 1
				 mesh->indices.push_back(start);
				 mesh->indices.push_back(start + 1);
				 mesh->indices.push_back(start + numColumns + 1);
				 mesh->indices.push_back(start + numColumns + 1);
				 mesh->indices.push_back(start + numColumns);
				 mesh->indices.push_back(start);
			 }
		 }
	 }
	 void createTorus(float innerRadius, float outerRadius, int numRings, int numRingSegments, MeshData* meshData)
	 {
		 meshData->vertices.clear();
		 meshData->indices.clear();

		 //VERTICES
		 float phiStep = (ew::PI * 2) / numRingSegments;
		 float thetaStep = (ew::PI * 2) / numRings;
		 for (size_t i = 0; i <= numRings; i++)
		 {
			 float theta = thetaStep * i;
			 for (size_t j = 0; j <= numRingSegments; j++)
			 {
				 float phi = phiStep * j;
				 Vertex v;
				 v.pos.x = cos(theta) * (outerRadius + cos(phi) * innerRadius);
				 v.pos.y = sin(theta) * (outerRadius + cos(phi) * innerRadius);
				 v.pos.z = sin(phi) * innerRadius;
				 v.normal.x = cos(theta) * cos(phi);
				 v.normal.y = sin(theta) * cos(phi);
				 v.normal.z = sin(phi);
				 v.normal = ew::Normalize(v.normal);
				 v.uv = ew::Vec2((float)j / numRingSegments, (float)i / numRings);
				 meshData->vertices.push_back(v);
			 }
		 }
		 //INDICES
		 //Stack
		 for (size_t stack = 0; stack < numRings; stack++)
		 {
			 //Slice
			 for (size_t slice = 0; slice <= numRingSegments; slice++)
			 {
				 int start = slice + stack * (numRingSegments+1);

				 int i1 = start;
				 int i2 = start + 1;
				 int i3 = start + 1 + numRingSegments;
				 int i4 = start + numRingSegments;

				 meshData->indices.push_back(i1);
				 meshData->indices.push_back(i3);
				 meshData->indices.push_back(i2);

				 meshData->indices.push_back(i3);
				 meshData->indices.push_back(i1);
				 meshData->indices.push_back(i4);
			 }
		 }
	 }
}