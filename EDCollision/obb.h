#pragma once

#include "EDCollision.h"

#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"

namespace EDCollision
{
	bool ObbToObb(const Obb& obb1, const Obb& obb2, Contact* contact = 0);
	bool ObbToObbSolution(const Obb& obb1, const Obb& obb2, Contact* contact = 0);

	bool ObbToTriangle(const Obb& obb, const Float3& a, const Float3& b, const Float3& c, Contact* contact = 0);
	bool ObbToTriangleSolution(const Obb& obb, const Float3& a, const Float3& b, const Float3& c, Contact* contact = 0);

	bool SloppyObbToAabb(const Obb& obb, const Aabb& aabb);
	bool SloppyObbToAabbSolution(const Obb& obb, const Aabb& aabb);
}