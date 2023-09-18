#include "particles.h"
#include "external/glad.h"

namespace ew {
	ParticleSystem::ParticleSystem(unsigned int capacity)
		:m_particleCount(0), m_capacity(capacity)
	{
		m_particles = std::vector<Particle>(capacity);

		std::fill(m_particles.begin(), m_particles.end(), Particle{ ew::Vec3(0),1.0f,ew::Vec4(1.0f),ew::Vec3(0.0f) });

		m_particlePositionsBuffer = std::vector<float>(capacity * 4);
		std::fill(m_particlePositionsBuffer.begin(), m_particlePositionsBuffer.end(), 0.0f);

		m_particleColorsBuffer = std::vector<float>(capacity * 4);
		std::fill(m_particleColorsBuffer.begin(), m_particleColorsBuffer.end(), 1.0f);

		static const GLfloat g_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 -0.5f, 0.5f, 0.0f,
		 0.5f, 0.5f, 0.0f,
		};
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		//Buffer of original vertex positions
		glGenBuffers(1, &m_geometryVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_geometryVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		// The VBO containing the positions and sizes of the particles
		glGenBuffers(1, &m_positionVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, capacity * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

		// The VBO containing the colors of the particles
		glGenBuffers(1, &m_colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, capacity * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

		// 1st attribute buffer : original vertex positions
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_geometryVBO);
		glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE,0,(void*)0);

		// 2nd attribute buffer (Positions(x,y,z,size))
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
		//Vertex attribute is vec4(x,y,z,size)
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//3rd attribute buffer (Color (RGBA)
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices.
		glVertexAttribDivisor(1, 1); // positions : one per quad
		glVertexAttribDivisor(2, 1); // color : one per quad 

		//Test of initial values
		//TODO: Move these to public "emit" function
		for (size_t i = 0; i < m_capacity; i++)
		{
			Particle& p = m_particles[i];

			float t = ((float)i / m_capacity);
			int numColumns = 100;
			int numRows = m_capacity / numColumns;

			int col = i % numColumns;
			int row = i / numColumns;

			float theta = ew::PI * 2.0 * (float)col / numColumns;
			float phi = ew::PI * (float)row / numRows;
			p.position = ew::Vec3(cosf(theta)*sinf(phi), cosf(phi), sinf(theta) * sinf(phi));
			p.velocity = ew::Normalize(p.position)*0.2f;
			p.size = 0.5f;
			p.color = ew::Vec4(ew::RandomRange(0, 1.0f), ew::RandomRange(0, 1.0f), ew::RandomRange(0, 1.0f), 1.0f);
		}
	}
	void ParticleSystem::draw(float deltaTime, ew::Shader* shader, const ew::Mat4& view, const ew::Mat4& projection, const ew::Vec3& cameraPos)
	{
		//Update
		m_particleCount = 0;
		for (size_t i = 0; i < m_capacity; i++)
		{
			Particle& p = m_particles[i];
			p.position += p.velocity * deltaTime;
			p.cameraDistance = ew::Magnitude(p.position - cameraPos);

			m_particlePositionsBuffer[m_particleCount * 4] = p.position.x;
			m_particlePositionsBuffer[m_particleCount * 4 + 1] = p.position.y;
			m_particlePositionsBuffer[m_particleCount * 4 + 2] = p.position.z;
			m_particlePositionsBuffer[m_particleCount * 4 + 3] = p.size;

			m_particleColorsBuffer[m_particleCount * 4] = p.color.x;
			m_particleColorsBuffer[m_particleCount * 4 +1] = p.color.y;
			m_particleColorsBuffer[m_particleCount * 4 + 2] = p.color.z;
			m_particleColorsBuffer[m_particleCount * 4 +3] = p.color.w;

			m_particleCount++;
		}
		std::sort(&m_particles[0], &m_particles[m_capacity-1]);
		//Draw
		shader->use();
		shader->setInt("_Billboard", m_billboard ? 1 : 0);
		shader->setMat4("_View", view);
		shader->setMat4("_Projection", projection);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
		glBufferData(GL_ARRAY_BUFFER, m_capacity * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); //Buffer orphaning
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_particleCount * sizeof(float) * 4, m_particlePositionsBuffer.data());

		glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
		glBufferData(GL_ARRAY_BUFFER, m_capacity * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); //Buffer orphaning
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_particleCount * sizeof(float) * 4, m_particleColorsBuffer.data());

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_particleCount);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}