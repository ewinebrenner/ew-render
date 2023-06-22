#pragma once
#include <math.h>

namespace ew {
	struct Vec3 {
		float x, y, z;

		Vec3() :x(0), y(0), z(0) {};
		Vec3(float x) :x(x), y(x), z(x) {};
		Vec3(float x, float y) :x(x), y(y), z(0) {};
		Vec3(float x, float y, float z) :x(x), y(y), z(z) {};

		//Operator overloads
		Vec3& operator+=(const Vec3& rhs);
		Vec3& operator-=(const Vec3& rhs);
		Vec3& operator*=(float rhs);
		Vec3& operator/=(float rhs);

		friend Vec3 operator+(Vec3 lhs, const Vec3& rhs);
		friend Vec3 operator-(Vec3 lhs, const Vec3& rhs);
		friend Vec3 operator*(Vec3 lhs, float rhs);
		friend Vec3 operator/(Vec3 lhs, float rhs);
	};

	//Operator overloads
	inline Vec3& Vec3::operator+=(const Vec3& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	inline Vec3& Vec3::operator-=(const Vec3& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}

	inline Vec3& Vec3::operator*=(float rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		return *this;
	}

	inline Vec3& Vec3::operator/=(float rhs)
	{
		*this *= (1.0f / rhs);
		return *this;
	}

	inline Vec3 operator+(Vec3 lhs, const Vec3& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	inline Vec3 operator-(Vec3 lhs, const Vec3& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	inline Vec3 operator*(Vec3 lhs, float rhs)
	{
		lhs *= rhs;
		return lhs;
	}

	inline Vec3 operator/(Vec3 lhs, float rhs)
	{
		lhs /= rhs;
		return lhs;
	}

	//Utility functions
	inline float Dot(const Vec3& a, const Vec3& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline Vec3 Cross(const Vec3& a, const Vec3& b) {
		return Vec3{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	inline float Magnitude(const Vec3& v)
	{
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	inline Vec3 Normalize(const Vec3& v)
	{
		float mag = Magnitude(v);
		if (mag == 0)
			return Vec3(0);
		return v / mag;
	}
}

