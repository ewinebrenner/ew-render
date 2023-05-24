#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"

struct aiMesh;

namespace ew {

	class Model {
	public:
		Model() {};
		bool loadFromFile(const char* filePath);
		void draw();
	private:
		std::vector<Mesh> m_meshes;
		Mesh convertMesh(aiMesh* mesh);
	};
}
