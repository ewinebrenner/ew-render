#pragma once
#include "vec4.h"
#include <cstddef>

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
		inline Vec4& operator[](int i) {
			return (*reinterpret_cast<Vec4*>(n[i]));
		}
		inline const Vec4& operator[](int i) const{
			return (*reinterpret_cast<const Vec4*>(n[i]));
		}
		inline friend Vec4 operator * (const Mat4& m, const Vec4& v) {
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
			m[0][0] = l[0][0] * r[0][0] + l[1][0] * r[0][1] + l[2][0] * r[0][2] + l[3][0] * r[0][3];//dot(l_row_0,r_col_0)
			m[1][0] = l[0][0] * r[1][0] + l[1][0] * r[1][1] + l[2][0] * r[1][2] + l[3][0] * r[1][3];//dot(l_row_0,r_col_1)
			m[2][0] = l[0][0] * r[2][0] + l[1][0] * r[2][1] + l[2][0] * r[2][2] + l[3][0] * r[2][3];//dot(l_row_0,r_col_2)
			m[3][0] = l[0][0] * r[3][0] + l[1][0] * r[3][1] + l[2][0] * r[3][2] + l[3][0] * r[3][3];//dot(l_row_0,r_col_3)
			// Row 1		  		    		  		    		  		    		  
			m[0][1] = l[0][1] * r[0][0] + l[1][1] * r[0][1] + l[2][1] * r[0][2] + l[3][1] * r[0][3];//dot(l_row_1,r_col_0)
			m[1][1] = l[0][1] * r[1][0] + l[1][1] * r[1][1] + l[2][1] * r[1][2] + l[3][1] * r[1][3];//dot(l_row_1,r_col_1)
			m[2][1] = l[0][1] * r[2][0] + l[1][1] * r[2][1] + l[2][1] * r[2][2] + l[3][1] * r[2][3];//dot(l_row_1,r_col_2)
			m[3][1] = l[0][1] * r[3][0] + l[1][1] * r[3][1] + l[2][1] * r[3][2] + l[3][1] * r[3][3];//dot(l_row_1,r_col_3)
			// Row  2		  		    		  		    		  		    		  
			m[0][2] = l[0][2] * r[0][0] + l[1][2] * r[0][1] + l[2][2] * r[0][2] + l[3][2] * r[0][3];//dot(l_row_2,r_col_0)
			m[1][2] = l[0][2] * r[1][0] + l[1][2] * r[1][1] + l[2][2] * r[1][2] + l[3][2] * r[1][3];//dot(l_row_2,r_col_1)
			m[2][2] = l[0][2] * r[2][0] + l[1][2] * r[2][1] + l[2][2] * r[2][2] + l[3][2] * r[2][3];//dot(l_row_2,r_col_2)
			m[3][2] = l[0][2] * r[3][0] + l[1][2] * r[3][1] + l[2][2] * r[3][2] + l[3][2] * r[3][3];//dot(l_row_2,r_col_3)
			// Row  3		 			 		 			 		 			 		    
			m[0][3] = l[0][3] * r[0][0] + l[1][3] * r[0][1] + l[2][3] * r[0][2] + l[3][3] * r[0][3];//dot(l_row_3,r_col_0)
			m[1][3] = l[0][3] * r[1][0] + l[1][3] * r[1][1] + l[2][3] * r[1][2] + l[3][3] * r[1][3];//dot(l_row_3,r_col_1)
			m[2][3] = l[0][3] * r[2][0] + l[1][3] * r[2][1] + l[2][3] * r[2][2] + l[3][3] * r[2][3];//dot(l_row_3,r_col_2)
			m[3][3] = l[0][3] * r[3][0] + l[1][3] * r[3][1] + l[2][3] * r[3][2] + l[3][3] * r[3][3];//dot(l_row_3,r_col_3)
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
}
