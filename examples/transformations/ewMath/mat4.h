#pragma once
#include "vec4.h"
#include <cstddef>

namespace ew {
	struct Mat4 {
		Vec4 v[4];
		Mat4() {
			v[0] = v[1] = v[2] = v[3] = Vec4(0);
		}
		Vec4& operator[](std::size_t idx);
		friend Vec4 operator*(Mat4& lhs, Vec4& rhs);
		//friend Mat4 operator*(Mat4& lhs, Mat4& rhs);
	};
	inline Vec4& ew::Mat4::operator[](std::size_t idx)
	{
		return v[idx];
	}
	//Operator overloads
	inline Vec4 operator*(Mat4& lhs, Vec4& rhs) {
		Vec4 v;
		v.x = lhs[0][0] * rhs.x + lhs[1][0] * rhs.y + lhs[2][0] * rhs.z + lhs[3][0] * rhs.w;
		v.y = lhs[0][1] * rhs.x + lhs[1][1] * rhs.y + lhs[2][1] * rhs.z + lhs[3][1] * rhs.w;
		v.z = lhs[0][2] * rhs.x + lhs[1][2] * rhs.y + lhs[2][2] * rhs.z + lhs[3][2] * rhs.w;
		v.w = lhs[0][3] * rhs.x + lhs[1][3] * rhs.y + lhs[2][3] * rhs.z + lhs[3][3] * rhs.w;
		return v;
	}
	inline Mat4 operator*(Mat4& a, Mat4& b) {
		Mat4 m;
		//Row 1
		m[0][0] = a[0][0] * b[0][0] + a[1][0] * b[0][1] + a[2][0] * b[0][2] + a[3][0] * b[0][3];
		m[1][0] = a[0][0] * b[1][0] + a[1][0] * b[1][1] + a[2][0] * b[1][2] + a[3][0] * b[1][3];
		m[2][0] = a[0][0] * b[2][0] + a[1][0] * b[2][1] + a[2][0] * b[2][2] + a[3][0] * b[2][3];
		m[3][0] = a[0][0] * b[3][0] + a[1][0] * b[3][1] + a[2][0] * b[3][2] + a[3][0] * b[3][3];
		return m;
	}

	//Utility functions 
	inline Mat4 Scale(float x, float y, float z) {
		Mat4 m;
		m[0][0] = x;
		m[1][1] = y;
		m[2][2] = z;
		m[3][3] = 1.0f;
		return m;
	}

	inline Mat4 Scale(float s) {
		return Scale(s,s,s);
	}

	inline Mat4 Mat4Identity() {
		return Scale(1);
	}

	inline Mat4 Translate(float x, float y, float z) {
		Mat4 m = Mat4Identity();
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
		return m;
	}
}
