#pragma once

#include "boundingVolume.h"
#include "Float4x4.h"

namespace EDMath
{
	struct Obb : BoundingVolume
	{
		static const Float3 unitVertices[8];
		static const unsigned int faceIndices[6][4];

		Float4x4 transform;
		Float3 extents;

		Type GetType(void)const{ return OBB; }
		Aabb GetAabb(void)const;
	};

	void ComputeObb(Obb& obb, const Float3 pts[], unsigned int numPts);

	void TransformObb(Obb& out, const Obb& in, const Float4x4& transform);
	void InvTransformObb(Obb& out, const Obb& in, const Float4x4& transform);
}