#pragma once

#include "EDCollision.h"

#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"

namespace EDCollision
{
	float ClosestPtAabb( const Float3& p, const Aabb& aabb, Float3& cp );
	float ClosestPtAabbSolution( const Float3& p, const Aabb& aabb, Float3& cp );

	bool AabbToAabb(const Aabb& aabb1, const Aabb& aabb2, Contact* contact = 0);
	bool AabbToAabbSolution(const Aabb& aabb1, const Aabb& aabb2, Contact* contact = 0);

	bool AabbToObb(const Aabb& aabb, const Obb& obb, Contact* contact = 0);
	bool AabbToObbSolution(const Aabb& aabb, const Obb& obb, Contact* contact = 0);
}