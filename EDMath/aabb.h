#pragma once

#include "boundingVolume.h"
#include "Float4x4.h"

namespace EDMath
{
	struct Aabb : BoundingVolume
	{
		Aabb(void){}
		Aabb(Float3 c, Float3 e) : center(c), extents(e) {}

		Float3 center;
		Float3 extents;

		Float3 Min(void)const{ return (center-extents); }
		Float3 Max(void)const{ return (center+extents); }

		Type GetType(void)const{ return AABB; }
		Aabb GetAabb(void)const{ return *this; }
	};

	void ComputeAabb(Aabb& aabb, const Float3 pts[], unsigned int numPts);

	void ComputeAabb(Aabb& aabb, const float* pts, unsigned int numPts, size_t offset);

	void TransformAabb(Aabb& out, const Aabb& in, const Float4x4& transform);
}