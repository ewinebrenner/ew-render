#pragma once
#include "Camera.h"

namespace ew {
	class FlyCamController {
	public:
		FlyCamController(Camera* camera);
		void moveForward(float delta);
		void moveRight(float delta);
		void moveUp(float delta);
		void addPitch(float delta);
		void addYaw(float delta);
		void updateCamRotation();
	private:
		Camera* m_camera = nullptr;
		float m_pitch = 0; //rotation around X axis in degrees
		float m_yaw = 0; //rotation around Y axis in degrees
	};
}