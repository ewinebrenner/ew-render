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
	inline Mat4 LookAtMatrix(const Vec3& eyePos, const Vec3& target, const Vec3& up) {
		Vec3 forward = Normalize(target-eyePos);
		Vec3 right = Normalize(Cross(forward,up));
		Vec3 u = Normalize(Cross(right,forward));
		Mat4 m = IdentityMatrix();
		m[0][0] = right.x;
		m[1][0] = right.y;
		m[2][0] = right.z;
		m[0][1] = u.x;
		m[1][1] = u.y;
		m[2][1] = u.z;
		m[0][2] = -forward.x;
		m[1][2] = -forward.y;
		m[2][2] = -forward.z;
		m[3][0] = -Dot(right, eyePos);
		m[3][1] = -Dot(u, eyePos);
		m[3][2] = Dot(forward, eyePos);
		return m;
	}
	/// <summary>
	/// A perspective projection matrix
	/// </summary>
	/// <param name="fov">Vertical field of view, in radians</param>
	/// <param name="aspectRatio">Aspect ratio (width/height)</param>
	/// <param name="nearPlane">Distance of near clipping plane(always positive)</param>
	/// <param name="farPlane">Distance of far clipping plane(always positive)</param>
	/// <returns></returns>
	inline Mat4 PerspectiveMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) {
		float c = tanf(fov / 2.0f);
		Mat4 m = Mat4(0);
		m[0][0] = 1.0f / (aspectRatio * c);
		m[1][1] = 1.0f / c;
		m[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
		m[2][3] = -1;
		m[3][2] = (2 * farPlane * nearPlane) / (nearPlane - farPlane);
		m[3][3] = 0;
		return m;
	}
	inline Mat4 OrthographicMatrix(float height, float aspectRatio, float nearPlane, float farPlane) {
		float width = height * aspectRatio;
		float r = width * 0.5f;
		float l = -r;
		float t = height * 0.5f;
		float b = -t;
		float n = nearPlane;
		float f = farPlane;
		Mat4 m = IdentityMatrix();
		m[0][0] = 2.0f / (r - l);
		m[1][1] = 2.0f / (t - b);
		m[2][2] = -2.0f / (f - n);
		m[3][0] = -(r + l) / (r - l);
		m[3][1] = -(t + b) / (t - b);
		m[3][2] = -(f + n) / (f - n);
		return m;
	}
}