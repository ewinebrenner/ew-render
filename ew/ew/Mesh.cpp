#include "Mesh.h"
#include <GL/glew.h>

ew::Mesh::Mesh() {

}
void ew::Mesh::load(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	if (!m_initialized) {
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glGenBuffers(1, &m_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

		//POSITION
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, position)));
		glEnableVertexAttribArray(0);

		//NORMAL
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(1);

		//UV
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, uv)));
		glEnableVertexAttribArray(2);

		//TANGENT
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, tangent)));
		glEnableVertexAttribArray(3);
		m_initialized = true;
	}
	
	glBindVertexArray(m_vao);

	//Supply vbo data
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	//Supply ebo data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	m_vertices = vertices;
	m_indices = indices;
}


void ew::Mesh::draw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}