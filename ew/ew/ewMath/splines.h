#pragma once
#include "ewMath.h"

namespace ew {
	inline ew::Vec3 evaluateQuadraticBezier(ew::Vec3 p0, ew::Vec3 p1, ew::Vec3 p2, float t) {
		ew::Vec3 p3 = ew::Lerp(p0, p1, t);
		ew::Vec3 p4 = ew::Lerp(p1, p2, t);
		return ew::Lerp(p3, p4, t);
	}

	inline ew::Vec3 evaluateCubicBezier(ew::Vec3 p0, ew::Vec3 p1, ew::Vec3 p2, ew::Vec3 p3, float t) {
		ew::Vec3 p4 = ew::Lerp(p0, p1, t);
		ew::Vec3 p5 = ew::Lerp(p1, p2, t);
		ew::Vec3 p6 = ew::Lerp(p2, p3, t);
		return evaluateQuadraticBezier(p4, p5, p6, t);
	}
	inline ew::Vec3 evaluateCubicBezierTangent(ew::Vec3 p0, ew::Vec3 p1, ew::Vec3 p2, ew::Vec3 p3, float t) {
		float oneMinusT = (1.0f - t);
		return 3.0f * oneMinusT * oneMinusT * (p1 - p0)
			+ 6.0f * t * oneMinusT * (p2 - p1)
			+ 3.0f * t * t * (p3 - p2);
	}
}
