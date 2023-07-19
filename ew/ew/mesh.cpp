#include "mesh.h"
#include "ewMath/ewMath.h"
#include "external/glad.h"

namespace ew {
	unsigned int createVAO(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//Define a new buffer id
		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//Allocate space for + send vertex data to GPU.
		if (vertices.size() > 0) {
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		}

		unsigned int ebo;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		if (indices.size() > 0) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
		}

		//Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, pos));
		glEnableVertexAttribArray(0);

		//Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		//UV attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, uv)));
		glEnableVertexAttribArray(2);

		return vao;
	}
}