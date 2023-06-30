#pragma once
#include "vec4.h"
#include <cstddef>

//namespace ew {
//	//Data layout is row-major for performance reasons
//	struct Mat4 {
//		float m00, m01, m02, m03; //col 1
//		float m10, m11, m12, m13; //col 2
//		float m20, m21, m22, m23; //col 3
//		float m30, m31, m32, m33; //col 4
//	};
//	inline Mat4 Mat4Identity() {
//		return {
//			1.0f, 0.0f, 0.0f, 0.0f,
//			0.0f, 1.0f, 0.0f, 0.0f,
//			0.0f, 0.0f, 1.0f, 0.0f,
//			0.0f, 0.0f, 0.0f, 1.0f
//		};
//	}
//
//	inline Vec4 Transform(const Mat4& m, const Vec4& v) {
//		Vec4 result = {};
//		result.x = m.m00 * v.x + m.m10 * v.y + m.m20 * v.z + m.m30 * v.w;
//		result.y = m.m01 * v.x + m.m11 * v.y + m.m21 * v.z + m.m31 * v.w;
//		result.z = m.m02 * v.x + m.m12 * v.y + m.m22 * v.z + m.m32 * v.w;
//		result.w = m.m03 * v.x + m.m13 * v.y + m.m23 * v.z + m.m33 * v.w;
//		return result;
//	}
//	inline Mat4 Mat4Multiply(const Mat4& l, const Mat4& r) {
//		Mat4 m;
//		//Row 0
//		m.m00 = l.m00 * r.m00 + l.m10 * r.m01 + l.m20 * r.m02 + l.m30 * r.m03; //dot(l_row_0,r_col_0)
//		m.m10 = l.m00 * r.m10 + l.m10 * r.m11 + l.m20 * r.m12 + l.m30 * r.m13; //dot(l_row_0,r_col_1)
//		m.m20 = l.m00 * r.m20 + l.m10 * r.m21 + l.m20 * r.m22 + l.m30 * r.m23; //dot(l_row_0,r_col_2)
//		m.m30 = l.m00 * r.m30 + l.m10 * r.m31 + l.m20 * r.m32 + l.m30 * r.m33; //dot(l_row_0,r_col_3)
//		// Row 1
//		m.m01 = l.m01 * r.m00 + l.m11 * r.m01 + l.m21 * r.m02 + l.m31 * r.m03; //dot(l_row_1,r_col_0)
//		m.m11 = l.m01 * r.m10 + l.m11 * r.m11 + l.m21 * r.m12 + l.m31 * r.m13; //dot(l_row_1,r_col_1)
//		m.m21 = l.m01 * r.m20 + l.m11 * r.m21 + l.m21 * r.m22 + l.m31 * r.m23; //dot(l_row_1,r_col_2)
//		m.m31 = l.m01 * r.m30 + l.m11 * r.m31 + l.m21 * r.m32 + l.m31 * r.m33; //dot(l_row_1,r_col_3)
//		// Row 2
//		m.m02 = l.m02 * r.m00 + l.m12 * r.m01 + l.m22 * r.m02 + l.m32 * r.m03; //dot(l_row_2,r_col_0)
//		m.m12 = l.m02 * r.m10 + l.m12 * r.m11 + l.m22 * r.m12 + l.m32 * r.m13; //dot(l_row_2,r_col_1)
//		m.m22 = l.m02 * r.m20 + l.m12 * r.m21 + l.m22 * r.m22 + l.m32 * r.m23; //dot(l_row_2,r_col_2)
//		m.m32 = l.m02 * r.m30 + l.m12 * r.m31 + l.m22 * r.m32 + l.m32 * r.m33; //dot(l_row_2,r_col_3)
//		// Row 3
//		m.m03 = l.m03 * r.m00 + l.m13 * r.m01 + l.m23 * r.m02 + l.m33 * r.m03; //dot(l_row_3,r_col_0)
//		m.m13 = l.m03 * r.m10 + l.m13 * r.m11 + l.m23 * r.m12 + l.m33 * r.m13; //dot(l_row_3,r_col_1)
//		m.m23 = l.m03 * r.m20 + l.m13 * r.m21 + l.m23 * r.m22 + l.m33 * r.m23; //dot(l_row_3,r_col_2)
//		m.m33 = l.m03 * r.m30 + l.m13 * r.m31 + l.m23 * r.m32 + l.m33 * r.m33; //dot(l_row_3,r_col_3)
//		return m;
//	}
//
//	inline Mat4 Translation(float x, float y, float z) {
//		return {
//			1.0f, 0.0f, 0.0f, 0.0f,
//			0.0f, 1.0f, 0.0f, 0.0f,
//			0.0f, 0.0f, 1.0f, 0.0f,
//			x, y, z, 1.0f
//		};
//	}
//	inline Mat4 Scale(float x, float y, float z) {
//		return {
//			x, 0.0f, 0.0f, 0.0f,
//			0.0f, y, 0.0f, 0.0f,
//			0.0f, 0.0f, z, 0.0f,
//			0.0f, 0.0f, 0.0f, 1.0f
//		};
//	}
//	inline Mat4 RotateX(float rad) {
//		Mat4 m = Mat4Identity();
//		const float cosA = cosf(rad);
//		const float sinA = sinf(rad);
//		m.m11 = cosA;
//		m.m21 = -sinA;
//		m.m12 = sinA;
//		m.m22 = cosA;
//		return m;
//	}
//
//	inline Mat4 RotateY(float rad) {
//		Mat4 m = Mat4Identity();
//		const float cosA = cosf(rad);
//		const float sinA = sinf(rad);
//		m.m00 = cosA;
//		m.m20 = sinA;
//		m.m02 = -sinA;
//		m.m22 = cosA;
//		return m;
//	}
//
//	inline Mat4 RotateZ(float rad) {
//		Mat4 m = Mat4Identity();
//		const float cosA = cosf(rad);
//		const float sinA = sinf(rad);
//		m.m00 = cosA;
//		m.m10 = -sinA;
//		m.m01 = sinA;
//		m.m11 = cosA;
//		return m;
//	}
//}

namespace ew {
	struct Mat4 {
	private:
		float n[4][4];
	public:
		Mat4() = default;
		Mat4(float n00, float n10, float n20, float n30,
			float n01, float n11, float n21, float n31,
			float n02, float n12, float n22, float n32,
			float n03, float n13, float n23, float n33)
		{
			n[0][0] = n00; n[1][0] = n10; n[2][0] = n20; n[3][0] = n30;
			n[0][1] = n01; n[1][1] = n11; n[2][1] = n21; n[3][1] = n31;
			n[0][2] = n02; n[1][2] = n12; n[2][2] = n22; n[3][2] = n32;
			n[0][3] = n03; n[1][3] = n13; n[2][3] = n23; n[3][3] = n33;
		};
		Mat4(const Vec4& a, const Vec4& b, const Vec4& c, const Vec4& d) {
			n[0][0] = a.x; n[0][1] = a.y; n[0][2] = a.z; n[0][3] = a.w;
			n[1][0] = b.x; n[1][1] = b.y; n[1][2] = b.z; n[1][3] = b.w;
			n[2][0] = c.x; n[2][1] = c.y; n[2][2] = c.z; n[2][3] = c.w;
			n[3][0] = d.x; n[3][1] = d.y; n[3][2] = d.z; n[3][3] = d.w;
		}
		Vec4& operator[](int i) {
			return (*reinterpret_cast<Vec4*>(n[i]));
		}
		const Vec4& operator[](int i) const{
			return (*reinterpret_cast<const Vec4*>(n[i]));
		}
		friend Vec4 operator *(const Mat4& m, const Vec4& v) {
			return Vec4(
				m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
				m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
				m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
				m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w
			);
		}
		inline friend Mat4 operator * (const Mat4& l, const Mat4& r) {
			Mat4 m;
			//Row 0
			m[0][0] = l[0][0]* r[0][0]+ l[1][0]* r[0][1]+ l[2][0]* r[0][2]+ l[3][0]* r[0][3];//dot(l_row_0,r_col_0)
			m[1][0] = l[0][0]* r[1][0]+ l[1][0]* r[1][1]+ l[2][0]* r[1][2]+ l[3][0]* r[1][3];//dot(l_row_0,r_col_1)
			m[2][0] = l[0][0]* r[2][0]+ l[1][0]* r[2][1]+ l[2][0]* r[2][2]+ l[3][0]* r[2][3];//dot(l_row_0,r_col_2)
			m[3][0] = l[0][0]* r[3][0]+ l[1][0]* r[3][1]+ l[2][0]* r[3][2]+ l[3][0]* r[3][3];//dot(l_row_0,r_col_3)
			// Row 1
			m[0][1]= l[0][1]* r[0][0]+ l[1][1]* r[0][1]+ l[2][1]* r[0][2]+ l[3][1]* r[0][3];//dot(l_row_1,r_col_0)
			m[1][1]= l[0][1]* r[1][0]+ l[1][1]* r[1][1]+ l[2][1]* r[1][2]+ l[3][1]* r[1][3];//dot(l_row_1,r_col_1)
			m[2][1]= l[0][1]* r[2][0]+ l[1][1]* r[2][1]+ l[2][1]* r[2][2]+ l[3][1]* r[2][3];//dot(l_row_1,r_col_2)
			m[3][1]= l[0][1]* r[3][0]+ l[1][1]* r[3][1]+ l[2][1]* r[3][2]+ l[3][1]* r[3][3];//dot(l_row_1,r_col_3)
			// Row 2
			m[0][2]= l[0][2]* r[0][0]+ l[1][2]* r[0][1]+ l[2][2]* r[0][2]+ l[3][2]* r[0][3];//dot(l_row_2,r_col_0)
			m[1][2]= l[0][2]* r[1][0]+ l[1][2]* r[1][1]+ l[2][2]* r[1][2]+ l[3][2]* r[1][3];//dot(l_row_2,r_col_1)
			m[2][2]= l[0][2]* r[2][0]+ l[1][2]* r[2][1]+ l[2][2]* r[2][2]+ l[3][2]* r[2][3];//dot(l_row_2,r_col_2)
			m[3][2]= l[0][2]* r[3][0]+ l[1][2]* r[3][1]+ l[2][2]* r[3][2]+ l[3][2]* r[3][3];//dot(l_row_2,r_col_3)
			// Row 3																	   
			m[0][3]= l[0][3]* r[0][0]+ l[1][3]* r[0][1]+ l[2][3]* r[0][2]+ l[3][3]* r[0][3];//dot(l_row_3,r_col_0)
			m[1][3]= l[0][3]* r[1][0]+ l[1][3]* r[1][1]+ l[2][3]* r[1][2]+ l[3][3]* r[1][3];//dot(l_row_3,r_col_1)
			m[2][3]= l[0][3]* r[2][0]+ l[1][3]* r[2][1]+ l[2][3]* r[2][2]+ l[3][3]* r[2][3];//dot(l_row_3,r_col_2)
			m[3][3]= l[0][3]* r[3][0]+ l[1][3]* r[3][1]+ l[2][3]* r[3][2]+ l[3][3]* r[3][3];//dot(l_row_3,r_col_3)
			return m;
		}
	};
	inline Mat4 IdentityMatrix() {
		return Mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
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
		Mat4 m = IdentityMatrix();
		const float cosA = cosf(rad);
		const float sinA = sinf(rad);
		m[1][1] = cosA;
		m[2][1] = -sinA;
		m[1][2] = sinA;
		m[2][2] = cosA;
		return m;
	}
	inline Mat4 RotateYMatrix(float rad) {
		Mat4 m = IdentityMatrix();
		const float cosA = cosf(rad);
		const float sinA = sinf(rad);
		m[0][0] = cosA;
		m[2][0] = sinA;
		m[0][2] = -sinA;
		m[2][2] = cosA;
		return m;
	}
	inline Mat4 RotateZMatrix(float rad) {
		Mat4 m = IdentityMatrix();
		const float cosA = cosf(rad);
		const float sinA = sinf(rad);
		m[0][0] = cosA;
		m[1][0] = -sinA;
		m[0][1] = sinA;
		m[1][1] = cosA;
		return m;
	}
}
