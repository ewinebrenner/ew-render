#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
namespace ew {
	struct Transform {
		glm::vec3 position = glm::vec3(0);
		glm::quat rotation = glm::quat(glm::vec3(0,0,0));
		glm::vec3 scale = glm::vec3(1);
		glm::mat4 localToWorld() const;
		glm::vec3 getForward()const;
		glm::vec3 getRight()const;
		glm::vec3 getUp()const;
		void translate(const glm::vec3& translation);
		void translate(float x, float y, float z);
		void rotate(float angleDegrees, const glm::vec3& axis);
		/// <summary>
		/// Get rotation expressed in euler angles (pitch,yaw,roll)
		/// </summary>
		/// <returns></returns>
		glm::vec3 getEulerAnglesRad()const;
	};
}