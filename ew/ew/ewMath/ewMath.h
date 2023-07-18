#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
#include <random>

namespace ew {
	constexpr float PI = (3.14159265359f);
	constexpr float DEG2RAD = (PI / 180.0f);
	constexpr float RAD2DEG = (180.0f / PI);
	inline float Radians(float degrees) {
		return degrees * DEG2RAD;
	}
	inline float RandomRange(float min, float max) {
		float t = (float)rand() / RAND_MAX;
		return min + (max - min) * t;
	}
}
