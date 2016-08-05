#include "obb.h"
#include "aabb.h"
#include "segment.h"
#include "../EDMath/polygon.h"

namespace EDCollision
{
	bool ObbToObb(const Obb& obb1, const Obb& obb2, Contact* contact)
	{
		return ObbToObbSolution( obb1, obb2, contact );
	}

	bool ObbToTriangle(const Obb& obb, const Float3& a, const Float3& b, const Float3& c, Contact* contact)
	{
		return ObbToTriangleSolution( obb, a, b, c, contact );
	}

	bool SloppyObbToAabb(const Obb& obb, const Aabb& aabb)
	{
		return SloppyObbToAabbSolution( obb, aabb );
	}
}