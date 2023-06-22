#pragma once
#include "vec3.h"

namespace ew {
	struct Vec4 {
		float x, y, z, w;
		Vec4();
		Vec4(float x);
		Vec4(float x, float y, float z, float w);
		Vec4(const Vec3& vec3, float w);

		//Operator overloads
		Vec4& operator+=(const Vec4& rhs);
		Vec4& operator-=(const Vec4& rhs);
		Vec4& operator*=(float rhs);

		friend Vec4 operator+(Vec4 lhs, const Vec4& rhs);
		friend Vec4 operator-(Vec4 lhs, const Vec4& rhs);
		friend Vec4 operator*(Vec4 lhs, float rhs);
	};

}

