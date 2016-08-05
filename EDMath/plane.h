#pragma once

#include "Float4x4.h"

namespace EDMath
{
	struct Plane
	{
		Float3 normal;
		float distance;
	};

	Plane ComputePlane(float a, float b, float c, float d);
	void ComputePlane(Plane& p, const Float3& a, const Float3& b, const Float3& c);
	void TransformPlane(Plane& out, const Plane& in, const Float4x4& transform);
}