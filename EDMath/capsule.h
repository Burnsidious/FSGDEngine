#pragma once

#include "boundingVolume.h"
#include "segment.h"
#include "Float4x4.h"

namespace EDMath
{
	struct Capsule : BoundingVolume
	{
		Float3 center;
		float radius;
		Float3 direction;
		float height;

		Type GetType(void)const{ return CAPSULE; }
		Aabb GetAabb(void)const;
	};

	void ComputeCapsule(Capsule& capsule, const Float3 pts[], unsigned int numPts);

	void TransformCapsule(Capsule& out, const Capsule& in, const Float4x4& transform);
}