#pragma once

#include "boundingVolume.h"
#include "Float4x4.h"

namespace EDMath
{
	struct Sphere : BoundingVolume
	{
		Float3 center;
		float radius;

		Type GetType(void)const{ return SPHERE; }
		Aabb GetAabb(void)const;
	};

	void ComputeSphere(Sphere &sphere, const Float3 pts[], unsigned int numPts);
	void ComputeSphere(Sphere &sphere, const float* pts, unsigned int numPts, size_t stride = 0);
	void BuildSphere(Sphere& out, const Sphere& a, const Sphere& b);

	void TransformSphere(Sphere &out, const Sphere& in, const Float4x4 transform);
}