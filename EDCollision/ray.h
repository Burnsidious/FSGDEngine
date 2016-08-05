#pragma once

#include "EDCollision.h"
#include "../EDMath/sphere.h"
#include "../EDMath/capsule.h"
#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"

namespace EDCollision
{
	bool OverlapRaySphere(Float3 start, Float3 direction, float maxLength, const Sphere& sphere);
	bool IntersectRaySphere(Float3 start, Float3 direction, float maxLength, const Sphere& sphere, Float3& cp, Float3& cn);

	bool OverlapRayCapsule(Float3 start, Float3 direction, float maxLength, const Capsule& capsule);
	bool IntersectRayCapsule(Float3 start, Float3 direction, float maxLength, const Capsule& capsule, Float3& cp, Float3& cn);

	bool OverlapRayAabb(Float3 start, Float3 direction, float maxLength, const Aabb& aabb);
	bool IntersectRayAabb(Float3 start, Float3 direction, float maxLength, const Aabb& aabb, Float3& cp, Float3& cn);

	bool OverlapRayObb(Float3 start, Float3 direction, float maxLength, const Obb& obb);
	bool IntersectRayObb(Float3 start, Float3 direction, float maxLength, const Obb& obb, Float3& cp, Float3& cn);

	bool OverlapRayCylinder(const Float3 &start, const Float3 &direction, float maxLength, const Float3 &p, const Float3 &q, float r);
	bool IntersectRayCylinder(const Float3 &start, const Float3 &direction, float maxLength, const Float3 &p, const Float3 &q, float r, Float3& cp, Float3& cn);

	bool IntersectRayTriangle(Float3 start, Float3 direction, float maxLength, Float3 a, Float3 b, Float3 c, Float3& bcCoords, float& t);


	bool OverlapRaySphereSolution(Float3 start, Float3 direction, float maxLength, const Sphere& sphere);
	bool IntersectRaySphereSolution(Float3 start, Float3 direction, float maxLength, const Sphere& sphere, Float3& cp, Float3& cn);

	bool OverlapRayCapsuleSolution(Float3 start, Float3 direction, float maxLength, const Capsule& capsule);
	bool IntersectRayCapsuleSolution(Float3 start, Float3 direction, float maxLength, const Capsule& capsule, Float3& cp, Float3& cn);

	bool OverlapRayAabbSolution(Float3 start, Float3 direction, float maxLength, const Aabb& aabb);
	bool IntersectRayAabbSolution(Float3 start, Float3 direction, float maxLength, const Aabb& aabb, Float3& cp, Float3& cn);

	bool OverlapRayObbSolution(Float3 start, Float3 direction, float maxLength, const Obb& obb);
	bool IntersectRayObbSolution(Float3 start, Float3 direction, float maxLength, const Obb& obb, Float3& cp, Float3& cn);

	bool OverlapRayCylinderSolution(const Float3 &start, const Float3 &direction, float maxLength, const Float3 &p, const Float3 &q, float r);
	bool IntersectRayCylinderSolution(const Float3 &start, const Float3 &direction, float maxLength, const Float3 &p, const Float3 &q, float r, Float3& cp, Float3& cn);

	bool IntersectRayTriangleSolution(Float3 start, Float3 direction, float maxLength, Float3 a, Float3 b, Float3 c, Float3& bcCoords, float& t);
}