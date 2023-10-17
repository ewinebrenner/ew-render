#include "camera.h"
#include "ewMath/transformations.h"

namespace ew {
	ew::Mat4 GetViewMatrix(const Camera& camera)
	{
		return ew::LookAtMatrix(camera.m_position, camera.m_target, ew::Vec3(0, 1, 0));
	}

	ew::Mat4 GetProjectionMatrix(const Camera& camera)
	{
		if (camera.m_orthographic) {
			return ew::OrthographicMatrix(camera.m_orthographicSize, camera.m_aspectRatio, camera.m_nearPlane, camera.m_farPlane);
		}
		else {
			return ew::PerspectiveMatrix(ew::Radians(camera.m_fov), camera.m_aspectRatio, camera.m_nearPlane, camera.m_farPlane);
		}
	}

	/// <summary>
	/// Moves a 3D camera with input vectors
	/// </summary>
	/// <param name="cameraController"></param>
	/// <param name="camera"></param>
	/// <param name="inputVectors">-1 to 1 on each axis(horizontal,vertical,forward)</param>
	/// <param name="deltaTime"></param>
	void MoveFlyCamera(CameraController* cameraController, Camera* camera, ew::Vec3 inputVectors, float deltaTime)
	{
		ew::Vec3 forward = ew::Normalize(camera->m_target - camera->m_position);
		ew::Vec3 right = ew::Normalize(ew::Cross(forward, ew::Vec3(0, 1, 0)));
		ew::Vec3 up = ew::Normalize(ew::Cross(forward, right));

		camera->m_position += inputVectors.x * right * cameraController->moveSpeed * deltaTime;
		camera->m_position += inputVectors.y * up * cameraController->moveSpeed * deltaTime;
		camera->m_position += inputVectors.z * forward * cameraController->moveSpeed * deltaTime;
	}
	void AimFlyCamera(CameraController* cameraController, double mouseX, double mouseY)
	{
		if (cameraController->firstMouse) {
			cameraController->prevMouseX = mouseX;
			cameraController->prevMouseY = mouseY;
			cameraController->firstMouse = false;
		}
		float deltaX = mouseX - cameraController->prevMouseX;
		float deltaY = mouseY - cameraController->prevMouseY;
		cameraController->yaw += deltaX * cameraController->mouseSensitivity;
		cameraController->pitch += -deltaY * cameraController->mouseSensitivity;
		if (cameraController->pitch < -89.9f) {
			cameraController->pitch = -89.9f;
		}
		else if (cameraController->pitch > 89.9f) {
			cameraController->pitch = 89.9f;
		}

		cameraController->prevMouseX = mouseX;
		cameraController->prevMouseY = mouseY;
	}
}

