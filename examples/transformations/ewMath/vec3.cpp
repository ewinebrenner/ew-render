#include "vec3.h"
#include <math.h>

namespace ew {
	Vec3::Vec3()
		:x(0),y(0),z(0)
	{

	}
	Vec3::Vec3(float x)
		:x(x),y(x),z(x)
	{

	}
	Vec3::Vec3(float x, float y)
		:x(x),y(y),z(0)
	{
	}
	Vec3::Vec3(float x, float y, float z)
		:x(x), y(y), z(z)
	{
	}
	Vec3& Vec3::operator+=(const Vec3& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	Vec3& Vec3::operator-=(const Vec3& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}

	Vec3& Vec3::operator*=(float rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		return *this;
	}

	Vec3& Vec3::operator/=(float rhs)
	{
		*this *= (1.0f / rhs);
		return *this;
	}

	Vec3 operator+(Vec3 lhs, const Vec3& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	Vec3 operator-(Vec3 lhs, const Vec3& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	Vec3 operator*(Vec3 lhs, float rhs)
	{
		lhs *= rhs;
		return lhs;
	}

	Vec3 operator/(Vec3 lhs, float rhs)
	{
		lhs /= rhs;
		return lhs;
	}


	float Dot(const Vec3& a, const Vec3& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	Vec3 Cross(const Vec3& a, const Vec3& b) {
		return Vec3{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}
	float Magnitude(const Vec3& v)
	{
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	}
	Vec3 Normalize(const Vec3& v)
	{
		float mag = Magnitude(v);
		if (mag == 0)
			return Vec3(0);
		return v / mag;
	}
}

