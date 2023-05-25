#pragma once
#include <glm/glm.hpp>
#include "Transform.h"

namespace ew {
	class Camera {
	public:
		Camera() {};
		glm::mat4 getViewMatrix()const;
		glm::mat4 getProjectionMatrix()const;
		Transform* getTransform();
		inline void setFov(float fovDeg) { m_fov = fovDeg; };
		inline void setAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; }
		inline void setNearPlane(float nearPlane) { m_nearPlane = nearPlane; }
		inline void setFarPlane(float farPlane) { m_farPlane = farPlane; }
	private:
		Transform m_transform = Transform();
		float m_fov = 60.0f; //Degrees
		float m_aspectRatio = 16.0f / 9.0f;
		float m_nearPlane = 0.01f;
		float m_farPlane = 100.0f;
	};
}