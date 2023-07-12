#pragma once
#include <math.h>

namespace ew {
	struct Vec2 {
		float x, y;

		Vec2() :x(0), y(0) {};
		Vec2(float x) :x(x), y(x) {};
		Vec2(float x, float y) :x(x), y(y) {};

		//Operator overloads
		Vec2& operator+=(const Vec2& rhs);
		Vec2& operator-=(const Vec2& rhs);
		Vec2& operator*=(float rhs);
		Vec2& operator/=(float rhs);

		friend Vec2 operator+(Vec2 lhs, const Vec2& rhs);
		friend Vec2 operator-(Vec2 lhs, const Vec2& rhs);
		friend Vec2 operator*(Vec2 lhs, float rhs);
		friend Vec2 operator/(Vec2 lhs, float rhs);
		friend Vec2 operator-(const Vec2& rhs);
	};

	//Operator overloads
	inline Vec2& Vec2::operator+=(const Vec2& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	inline Vec2& Vec2::operator-=(const Vec2& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}

	inline Vec2& Vec2::operator*=(float rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		return *this;
	}

	inline Vec2& Vec2::operator/=(float rhs)
	{
		*this *= (1.0f / rhs);
		return *this;
	}

	inline Vec2 operator+(Vec2 lhs, const Vec2& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	inline Vec2 operator-(Vec2 lhs, const Vec2& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	inline Vec2 operator*(Vec2 lhs, float rhs)
	{
		lhs *= rhs;
		return lhs;
	}

	inline Vec2 operator/(Vec2 lhs, float rhs)
	{
		lhs /= rhs;
		return lhs;
	}

	inline Vec2 operator-(const Vec2& rhs)
	{
		return rhs * -1.0f;
	}

	//Utility functions
	inline float Dot(const Vec2& a, const Vec2& b) {
		return a.x * b.x + a.y * b.y;
	}

	inline float Magnitude(const Vec2& v)
	{
		return sqrtf(v.x * v.x + v.y * v.y);
	}

	inline Vec2 Normalize(const Vec2& v)
	{
		float mag = Magnitude(v);
		if (mag == 0)
			return Vec2(0);
		return v / mag;
	}
}

