#include "aabb.h"
#include "obb.h"

namespace EDCollision
{
	float ClosestPtAabb( const Float3& p, const Aabb& aabb, Float3& cp )
	{
		return ClosestPtAabbSolution( p, aabb, cp );
	}

	bool AabbToAabb(const Aabb& aabb1, const Aabb& aabb2, Contact* contact)
	{
		return AabbToAabbSolution( aabb1, aabb2, contact );
	}

	bool AabbToObb(const Aabb& aabb, const Obb& obb, Contact* contact)
	{
		return AabbToObbSolution(aabb, obb, contact);
	}
}
