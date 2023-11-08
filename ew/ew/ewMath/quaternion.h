#pragma once
#include "vec3.h"
#include "mat4.h"

namespace ew {
	struct Quaternion {
		float x, y, z, w;
		Quaternion() = default;
		Quaternion(float x, float y, float z, float w) :
			x(x), y(y), z(z), w(w) {
		};
		const Vec3& GetVectorPart(void)const {
			return (reinterpret_cast<const Vec3&>(x));
		}

		friend Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
		friend Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

	};

	inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) {
		//TODO
		return {};
	}
	inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) {
		//TODO
		return {};
	}

	//HELPER FUNCTIONS

	inline Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) {
		//TODO
		return {};
	}

	inline Mat4 ToRotationMatrix(const Quaternion& q) {
		//TODO
		return {};
	}

	inline Vec3 RotateVec3(const Vec3& v, const Quaternion& q) {

	}
}