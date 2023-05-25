#include "Transform.h"
#include <glm/gtx/transform.hpp>

glm::mat4 ew::Transform::localToWorld() const
{
	glm::mat4 m = glm::scale(glm::mat4(1), scale);
	m = glm::toMat4(rotation) * m;
	m = glm::translate(m, position);
	return m;
}

glm::vec3 ew::Transform::getForward() const
{
	return rotation * glm::vec3(0, 0, 1);
}

glm::vec3 ew::Transform::getRight() const
{
	return rotation * glm::vec3(1, 0, 0);
}

glm::vec3 ew::Transform::getUp() const
{
	return rotation * glm::vec3(0, 1, 0);
}

void ew::Transform::translate(const glm::vec3& translation)
{
	position+=translation;
}

void ew::Transform::translate(float x, float y, float z)
{
	position += glm::vec3(x, y, z);
}

void ew::Transform::rotate(float angleDegrees, const glm::vec3& axis)
{
	rotation = glm::rotate(rotation, glm::radians(angleDegrees), axis);
}

glm::vec3 ew::Transform::getEulerAnglesRad() const
{
	return glm::eulerAngles(rotation);
}
