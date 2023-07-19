#pragma once
#include "ewMath/ewMath.h"
#include "ewMath/transformations.h"
namespace ew {
	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f); //Euler angles (Degrees)
		ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);

		ew::Mat4 getModelMatrix() const{
			return ew::TranslationMatrix(position.x, position.y, position.z)
				* ew::RotateYMatrix(ew::Radians(rotation.y))
				* ew::RotateXMatrix(ew::Radians(rotation.x))
				* ew::RotateZMatrix(ew::Radians(rotation.z))
				* ew::ScaleMatrix(scale.x, scale.y, scale.z);
		}
	};
}
