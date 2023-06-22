#include "vec4.h"

namespace ew {
	Vec4::Vec4()
		:x(0), y(0), z(0)
	{

	}

	Vec4::Vec4(float x)
		:x(x), y(x), z(x)
	{

	}

	Vec4::Vec4(float x, float y, float z, float w)
		:x(x), y(y), z(z), w(w)
	{
	}

	Vec4::Vec4(const Vec3& vec3, float w)
		:x(vec3.x),y(vec3.y),z(vec3.z),w(w)
	{

	}

	Vec4& Vec4::operator+=(const Vec4& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	Vec4& Vec4::operator-=(const Vec4& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}

	Vec4& Vec4::operator*=(float rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		return *this;
	}

	Vec4 operator+(Vec4 lhs, const Vec4& rhs)
	{
		lhs += rhs;
		return lhs;
	}

	Vec4 operator-(Vec4 lhs, const Vec4& rhs)
	{
		lhs -= rhs;
		return lhs;
	}

	Vec4 operator*(Vec4 lhs, float rhs)
	{
		lhs *= rhs;
		return lhs;
	}
}

