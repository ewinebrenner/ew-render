#pragma once
#include "ewMath/mat4.h"

namespace ew {
	inline Mat4 TranslationMatrix(float x, float y, float z) {
		return Mat4(
			1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0f, 0.0f, y,
			0.0f, 0.0f, 1.0f, z,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
	inline Mat4 ScaleMatrix(float x, float y, float z) {
		return Mat4(
			x, 0.0f, 0.0f, 0.0f,
			0.0f, y, 0.0f, 0.0f,
			0.0f, 0.0f, z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
	inline Mat4 RotateXMatrix(float rad) {
		const float cosA = cosf(rad);
		const float sinA = sinf(rad);
		Mat4 m = Mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, cosA, -sinA, 0.0f,
			0.0f, sinA, cosA, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		return m;
	}
	inline Mat4 RotateYMatrix(float rad) {
		const float cosA = cosf(rad);
		const float sinA = sinf(rad);
		Mat4 m = Mat4(
			cosA, 0.0f, sinA, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-sinA, 0.0f, cosA, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		return m;
	}
	inline Mat4 RotateZMatrix(float rad) {
		const float cosA = cosf(rad);
		const float sinA = sinf(rad);
		Mat4 m = Mat4(
			cosA, -sinA, 0.0f, 0.0f,
			sinA, cosA, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		return m;
	}
}