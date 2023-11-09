#pragma once
#include "ewMath/ewMath.h"
#include <assimp/Importer.hpp>
#include <assimp/quaternion.h>
#include "ewMath/quaternion.h"

namespace ew {
    inline ew::Vec3 aiVec3(const aiVector3D& v) {
        return ew::Vec3(v.x, v.y, v.z);
    }
    inline ew::Vec2 aiVec2(const aiVector3D& v) {
        return ew::Vec2(v.x, v.y);
    }
    inline ew::Mat4 aiMat4(const aiMatrix4x4& m) {
        return ew::Mat4(
            m.a1, m.a2, m.a3, m.a4,
            m.b1, m.b2, m.b3, m.b4,
            m.c1, m.c2, m.c3, m.c4,
            m.d1, m.d2, m.d3, m.d4
        );
    }
    inline ew::Quaternion aiQuat(const aiQuaternion& q) {
        return ew::Quaternion(q.x, q.y, q.z, q.w);
    }

}