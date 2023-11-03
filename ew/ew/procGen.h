#pragma once
#include "mesh.h"

namespace ew {
	 void createCube(float size, MeshData* meshData);
	 void createSphereOld(float radius, int numSegments, MeshData* meshData);
	 void createSphere(float radius, int numSegments, MeshData* meshData);
	 void createCylinder(float height, float radius, int numSegments, MeshData* meshData);
	 void createCone(float height, float radius, int numSegments, MeshData* meshData);
	 void createPlane(float size, int subdivisions, MeshData* meshData);
	 void createTorus(float innerRadius, float outerRadius, int numRings, int numRingSegments, MeshData* meshData);
}
