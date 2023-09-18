#pragma once
#include "ewMath/ewMath.h"
#include "shader.h"
#include <vector>

namespace ew {
	struct Particle {
		ew::Vec3 position;
		float size;
		ew::Vec4 color;
		ew::Vec3 velocity;
		float cameraDistance = -1.0f;
		bool operator<(Particle& that) {
			// Sort in reverse order : far particles drawn first.
			return this->cameraDistance > that.cameraDistance;
		}
	};
	class ParticleSystem {
	public:
		ParticleSystem(unsigned int capacity);
		inline void setBillboarding(bool v) { m_billboard = v; }
		inline int getNumParticles()const { return m_particleCount; }
		void draw(float deltaTime, Shader* shader, const ew::Mat4& view, const ew::Mat4& projection, const ew::Vec3& cameraPos);
	private:
		unsigned int m_vao;
		unsigned int m_geometryVBO;
		unsigned int m_positionVBO;
		unsigned int m_colorVBO;
		unsigned int m_capacity; //Max number of particles
		unsigned int m_particleCount; //Number of active particles
		std::vector<Particle> m_particles;
		std::vector<float> m_particlePositionsBuffer; //4 floats per particle(position.xyz,size)
		std::vector<float> m_particleColorsBuffer; //4 floats per particle(rgba)
		bool m_billboard = true;
	};
}