#include "ray.h"

#include "EDCollision.h"
#include "../EDMath/sphere.h"
#include "../EDMath/capsule.h"
#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"
#include "segment.h"

#ifndef SameSign
#define SameSign(a,b) ( (((unsigned int&)(a)) & 0x80000000) == (((unsigned int&)(b)) & 0x80000000) )
#endif 

namespace EDCollision
{
	bool OverlapRaySphere(Float3 start, Float3 direction, float maxLength, const Sphere& sphere)
	{
		return OverlapRaySphereSolution( start, direction, maxLength, sphere );
	}

	bool IntersectRaySphere(Float3 start, Float3 direction, float maxLength, const Sphere& sphere, Float3& cp, Float3& cn)
	{
		return IntersectRaySphereSolution( start, direction, maxLength, sphere, cp, cn );
	}

	bool OverlapRayCapsule(Float3 start, Float3 direction, float maxLength, const Capsule& capsule)
	{
		return OverlapRayCapsuleSolution( start, direction, maxLength, capsule );
	}

	bool IntersectRayCapsule(Float3 start, Float3 direction, float maxLength, const Capsule& capsule, Float3& cp, Float3& cn)
	{
		return IntersectRayCapsuleSolution( start, direction, maxLength, capsule, cp, cn );
	}

	bool OverlapRayAabb(Float3 start, Float3 direction, float maxLength, const Aabb& aabb)
	{
		return OverlapRayAabbSolution( start, direction, maxLength, aabb );
	}

	bool IntersectRayAabb(Float3 start, Float3 direction, float maxLength, const Aabb& aabb, Float3& cp, Float3& cn)
	{
		return IntersectRayAabbSolution( start, direction, maxLength, aabb, cp, cn );
	}

	bool OverlapRayObb(Float3 start, Float3 direction, float maxLength, const Obb& obb)
	{
		return OverlapRayObbSolution( start, direction, maxLength, obb );
	}

	bool IntersectRayObb(Float3 start, Float3 direction, float maxLength, const Obb& obb, Float3& cp, Float3& cn)
	{
		return IntersectRayObbSolution( start, direction, maxLength, obb, cp, cn );
	}

	bool IntersectRayTriangle(Float3 start, Float3 direction, float maxLength, Float3 a, Float3 b, Float3 c, Float3& bcCoords, float& t)
	{
		return IntersectRayTriangleSolution( start, direction, maxLength, a, b, c, bcCoords, t );
	}

	bool OverlapRayCylinder(const Float3 &start, const Float3 &direction, float maxLength, const Float3 &p, const Float3 &q, float r)
	{
		return OverlapRayCylinderSolution( start, direction, maxLength, p, q, r );
	}

	bool IntersectRayCylinder(const Float3 &start, const Float3 &direction, float maxLength, const Float3 &p, const Float3 &q, float r, Float3& cp, Float3& cn)
	{
		return IntersectRayCylinderSolution( start, direction, maxLength, p, q, r, cp, cn );
	}
}