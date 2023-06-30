#pragma once
#include <math.h>
#include "vec3.h"

namespace ew {
	struct Vec4 {
		float x, y, z, w;

		Vec4() :x(0), y(0), z(0), w(0) {};
		Vec4(float x) :x(x), y(x), z(x), w(x) {};
		Vec4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {};
		Vec4(const Vec3& v, float w) :x(v.x), y(v.y), z(v.z), w(w) {};

		//Operator overloads
		Vec4& operator+=(const Vec4& rhs);
		Vec4& operator-=(const Vec4& rhs);
		Vec4& operator*=(float rhs);
		Vec4& operator/=(float rhs);

		friend Vec4 operator+(Vec4 lhs, const Vec4& rhs);
		friend Vec4 operator-(Vec4 lhs, const Vec4& rhs);
		friend Vec4 operator*(Vec4 lhs, float rhs);
		friend Vec4 operator/(Vec4 lhs, float rhs);

		float& operator[](int i);
		const float& operator[](int i)const;
	};
	inline float& ew::Vec4::operator[](int i)
	{
		return ((&x)[i]);
	}
	inline const float& Vec4::operator[](int i) const
	{
		return ((&x)[i]);
	}
	//Operator overloads
	inline Vec4& Vec4::operator+=(const Vec4& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	inline Vec4& Vec4::operator-=(const Vec4& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}

	inline Vec4& Vec4::operator*=(float rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		return *this;
	}

	inline Vec4& Vec4::operator/=(float rhs)
	{
		*this *= (1.0f / rhs);
		return *this;
	}

	inline Vec4 operator+(Vec4 lhs, const Vec4& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	inline Vec4 operator-(Vec4 lhs, const Vec4& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	inline Vec4 operator*(Vec4 lhs, float rhs)
	{
		lhs *= rhs;
		return lhs;
	}

	inline Vec4 operator/(Vec4 lhs, float rhs)
	{
		lhs /= rhs;
		return lhs;
	}

	//Utility functions
	inline float Dot(const Vec4& a, const Vec4& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	inline float Magnitude(const Vec4& v)
	{
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

	inline Vec4 Normalize(const Vec4& v)
	{
		float mag = Magnitude(v);
		if (mag == 0)
			return Vec4(0);
		return v / mag;
	}
}

