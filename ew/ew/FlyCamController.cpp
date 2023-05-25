#include "FlyCamController.h"

ew::FlyCamController::FlyCamController(Camera* camera)
{
	m_camera = camera;
}

void ew::FlyCamController::moveForward(float delta)
{
	Transform* t = m_camera->getTransform();
	t->translate(t->getForward() * delta);
}

void ew::FlyCamController::moveRight(float delta)
{
	Transform* t = m_camera->getTransform();
	t->translate(t->getRight() * delta);
}

void ew::FlyCamController::moveUp(float delta)
{
	Transform* t = m_camera->getTransform();
	t->translate(t->getUp() * delta);
}

void ew::FlyCamController::addPitch(float delta)
{
	m_pitch += delta;
	m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
}

void ew::FlyCamController::addYaw(float delta)
{
	m_yaw += delta;
}

void ew::FlyCamController::updateCamRotation()
{
	glm::quat rotationX = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));
	glm::quat rotationY = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 1, 0));
	m_camera->getTransform()->rotation = rotationY * rotationX;
}
