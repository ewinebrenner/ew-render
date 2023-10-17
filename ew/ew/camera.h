#pragma once
#include "ewMath/ewMath.h"

namespace ew {
	class Camera {
	public:
		ew::Vec3 m_position = ew::Vec3(0);
		ew::Vec3 m_target = ew::Vec3(0, 0, 1.0f);
		float m_fov = 60.0f; // Vertical FOV (degrees)
		bool m_orthographic = false;
		float m_orthographicSize = 15.0f; //Height in viewspace
		float m_aspectRatio = 1.777f;
		float m_nearPlane = 0.1f;
		float m_farPlane = 1000.0f;
		ew::Vec4 m_bgColor = ew::Vec4(0.1f,0.12f,0.15f,1.0f);
	};

	struct CameraController {
		float moveSpeed = 5.0f;
		float yaw = -90.0f;
		float pitch = 0.0f;
		double prevMouseX = 0.0, prevMouseY = 0.0;
		bool firstMouse = true;
		float mouseSensitivity = 0.1f;
	};
	ew::Mat4 GetViewMatrix(const Camera& camera);
	ew::Mat4 GetProjectionMatrix(const Camera& camera);
	void MoveFlyCamera(CameraController* cameraController, Camera* camera, ew::Vec3 inputVectors, float deltaTime);
	void AimFlyCamera(CameraController* cameraController, double xpos, double ypos);
}