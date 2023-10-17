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
	inline float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}
	inline Vec3 Lerp(Vec3 a, Vec3 b, float t) {
		return a + (b - a) * t;
	}
	inline Vec4 Lerp(Vec4 a, Vec4 b, float t) {
		return a + (b - a) * t;
	}
	inline float InverseLerp(float a, float b, float v) {
		return (v - a) / (b - a);
	}
}
