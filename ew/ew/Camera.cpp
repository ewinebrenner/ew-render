#include "Camera.h"

glm::mat4 ew::Camera::getViewMatrix() const
{
	return glm::lookAt(m_transform.position, m_transform.position + m_transform.getForward(), glm::vec3(0, 1, 0));
}

glm::mat4 ew::Camera::getProjectionMatrix() const
{
	//TODO: Add orthographic support
	return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
}

ew::Transform* ew::Camera::getTransform()
{
	return &m_transform;
}
