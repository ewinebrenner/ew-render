#pragma once
#include "vec4.h"
#include <cstddef>

namespace ew {
	//Data layout is row-major for performance reasons
	struct Mat4 {
		float m00, m01, m02, m03; //col 1
		float m10, m11, m12, m13; //col 2
		float m20, m21, m22, m23; //col 3
		float m30, m31, m32, m33; //col 4
	};
	inline Mat4 Mat4Identity() {
		return {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	inline Vec4 Transform(const Mat4& m, const Vec4& v) {
		Vec4 result = {};
		result.x = m.m00 * v.x + m.m10 * v.y + m.m20 * v.z + m.m30 * v.w;
		result.y = m.m01 * v.x + m.m11 * v.y + m.m21 * v.z + m.m31 * v.w;
		result.z = m.m02 * v.x + m.m12 * v.y + m.m22 * v.z + m.m32 * v.w;
		result.w = m.m03 * v.x + m.m13 * v.y + m.m23 * v.z + m.m33 * v.w;
		return result;
	}
	inline Mat4 Mat4Multiply(const Mat4& l, const Mat4& r) {
		Mat4 m;
		//Row 0
		m.m00 = l.m00 * r.m00 + l.m10 * r.m01 + l.m20 * r.m02 + l.m30 * r.m03; //dot(l_row_0,r_col_0)
		m.m10 = l.m00 * r.m10 + l.m10 * r.m11 + l.m20 * r.m12 + l.m30 * r.m13; //dot(l_row_0,r_col_1)
		m.m20 = l.m00 * r.m20 + l.m10 * r.m21 + l.m20 * r.m22 + l.m30 * r.m23; //dot(l_row_0,r_col_2)
		m.m30 = l.m00 * r.m30 + l.m10 * r.m31 + l.m20 * r.m32 + l.m30 * r.m33; //dot(l_row_0,r_col_3)
		// Row 1
		m.m01 = l.m01 * r.m00 + l.m11 * r.m01 + l.m21 * r.m02 + l.m31 * r.m03; //dot(l_row_1,r_col_0)
		m.m11 = l.m01 * r.m10 + l.m11 * r.m11 + l.m21 * r.m12 + l.m31 * r.m13; //dot(l_row_1,r_col_1)
		m.m21 = l.m01 * r.m20 + l.m11 * r.m21 + l.m21 * r.m22 + l.m31 * r.m23; //dot(l_row_1,r_col_2)
		m.m31 = l.m01 * r.m30 + l.m11 * r.m31 + l.m21 * r.m32 + l.m31 * r.m33; //dot(l_row_1,r_col_3)
		// Row 2
		m.m02 = l.m02 * r.m00 + l.m12 * r.m01 + l.m22 * r.m02 + l.m32 * r.m03; //dot(l_row_2,r_col_0)
		m.m12 = l.m02 * r.m10 + l.m12 * r.m11 + l.m22 * r.m12 + l.m32 * r.m13; //dot(l_row_2,r_col_1)
		m.m22 = l.m02 * r.m20 + l.m12 * r.m21 + l.m22 * r.m22 + l.m32 * r.m23; //dot(l_row_2,r_col_2)
		m.m32 = l.m02 * r.m30 + l.m12 * r.m31 + l.m22 * r.m32 + l.m32 * r.m33; //dot(l_row_2,r_col_3)
		// Row 3
		m.m03 = l.m03 * r.m00 + l.m13 * r.m01 + l.m23 * r.m02 + l.m33 * r.m03; //dot(l_row_3,r_col_0)
		m.m13 = l.m03 * r.m10 + l.m13 * r.m11 + l.m23 * r.m12 + l.m33 * r.m13; //dot(l_row_3,r_col_1)
		m.m23 = l.m03 * r.m20 + l.m13 * r.m21 + l.m23 * r.m22 + l.m33 * r.m23; //dot(l_row_3,r_col_2)
		m.m33 = l.m03 * r.m30 + l.m13 * r.m31 + l.m23 * r.m32 + l.m33 * r.m33; //dot(l_row_3,r_col_3)
		return m;
	}

	inline Mat4 Translation(float x, float y, float z) {
		return {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			x, y, z, 1.0f
		};
	}
	inline Mat4 Scale(float x, float y, float z) {
		return {
			x, 0.0f, 0.0f, 0.0f,
			0.0f, y, 0.0f, 0.0f,
			0.0f, 0.0f, z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}
	inline Mat4 RotateX(float rad) {
		Mat4 m = Mat4Identity();
		const float cosA = cosf(rad);
		const float sinA = sinf(rad);
		m.m11 = cosA;
		m.m21 = -sinA;
		m.m12 = sinA;
		m.m22 = cosA;
		return m;
	}

	inline Mat4 RotateY(float rad) {
		Mat4 m = Mat4Identity();
		const float cosA = cosf(rad);
		const float sinA = sinf(rad);
		m.m00 = cosA;
		m.m20 = sinA;
		m.m02 = -sinA;
		m.m22 = cosA;
		return m;
	}

	inline Mat4 RotateZ(float rad) {
		Mat4 m = Mat4Identity();
		const float cosA = cosf(rad);
		const float sinA = sinf(rad);
		m.m00 = cosA;
		m.m10 = -sinA;
		m.m01 = sinA;
		m.m11 = cosA;
		return m;
	}
}

//namespace ew {
//	struct Mat4 {
//		Vec4 v[4];
//		Mat4() {
//			v[0] = v[1] = v[2] = v[3] = Vec4(0);
//		}
//		Vec4& operator[](std::size_t idx);
//		friend Vec4 operator*(Mat4& lhs, Vec4& rhs);
//		//friend Mat4 operator*(Mat4& lhs, Mat4& rhs);
//	};
//	inline Vec4& ew::Mat4::operator[](std::size_t idx)
//	{
//		return v[idx];
//	}
//	//Operator overloads
//	inline Vec4 operator*(Mat4& lhs, Vec4& rhs) {
//		Vec4 v;
//		v.x = lhs[0][0] * rhs.x + lhs[1][0] * rhs.y + lhs[2][0] * rhs.z + lhs[3][0] * rhs.w;
//		v.y = lhs[0][1] * rhs.x + lhs[1][1] * rhs.y + lhs[2][1] * rhs.z + lhs[3][1] * rhs.w;
//		v.z = lhs[0][2] * rhs.x + lhs[1][2] * rhs.y + lhs[2][2] * rhs.z + lhs[3][2] * rhs.w;
//		v.w = lhs[0][3] * rhs.x + lhs[1][3] * rhs.y + lhs[2][3] * rhs.z + lhs[3][3] * rhs.w;
//		return v;
//	}
//	inline Mat4 operator*(Mat4& a, Mat4& b) {
//		Mat4 m;
//		//Row 1
//		m[0][0] = a[0][0] * b[0][0] + a[1][0] * b[0][1] + a[2][0] * b[0][2] + a[3][0] * b[0][3];
//		m[1][0] = a[0][0] * b[1][0] + a[1][0] * b[1][1] + a[2][0] * b[1][2] + a[3][0] * b[1][3];
//		m[2][0] = a[0][0] * b[2][0] + a[1][0] * b[2][1] + a[2][0] * b[2][2] + a[3][0] * b[2][3];
//		m[3][0] = a[0][0] * b[3][0] + a[1][0] * b[3][1] + a[2][0] * b[3][2] + a[3][0] * b[3][3];
//		return m;
//	}
//
//	//Utility functions 
//	inline Mat4 Scale(float x, float y, float z) {
//		Mat4 m;
//		m[0][0] = x;
//		m[1][1] = y;
//		m[2][2] = z;
//		m[3][3] = 1.0f;
//		return m;
//	}
//
//	inline Mat4 Scale(float s) {
//		return Scale(s,s,s);
//	}
//
//	inline Mat4 Mat4Identity() {
//		return Scale(1);
//	}
//
//	inline Mat4 Translate(float x, float y, float z) {
//		Mat4 m = Mat4Identity();
//		m[3][0] = x;
//		m[3][1] = y;
//		m[3][2] = z;
//		return m;
//	}
//}
