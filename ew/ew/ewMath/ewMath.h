#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
namespace ew {
	constexpr float PI = (3.14159265359f);
	constexpr float DEG2RAD = (PI / 180.0f);
	constexpr float RAD2DEG = (180.0f / PI);
	inline float Radians(float degrees) {
		return degrees * DEG2RAD;
	}
}
