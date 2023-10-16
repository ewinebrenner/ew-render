#pragma once
#include "mesh.h"

namespace ew {
	 void createCube(float size, MeshData* meshData);
	 void createSphere(float radius, int numSegments, MeshData* meshData);
	 void createCylinder(float height, float radius, int numSegments, MeshData* meshData);
	 void createPlane(float size, int numSegments, MeshData* meshData);
	 void createTorus(float innerRadius, float outerRadius, int numRings, int numRingSegments, MeshData* meshData);
}
