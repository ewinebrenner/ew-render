#pragma once
namespace ew {
	struct Vec3 {
		float x, y, z;

		Vec3();
		Vec3(float x);
		Vec3(float x, float y);
		Vec3(float x, float y, float z);

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

	//Utility functions
	float Dot(const Vec3& a, const Vec3& b);
	Vec3 Cross(const Vec3& a, const Vec3& b);
	float Magnitude(const Vec3& v);
	Vec3 Normalize(const Vec3& v);
}

