#pragma once

#include "EDCollision.h"
#include "../EDMath/plane.h"
#include "../EDMath/sphere.h"
#include "../EDMath/capsule.h"
#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"

namespace EDCollision
{
	float ClosestPtSphere( const Sphere& s, const Float3& p, Float3& cp );

	bool SphereToSphere( const Sphere& sphere1, const Sphere& sphere2, Contact* contact = 0);

	bool SphereToCapsule( const Sphere& sphere, const Capsule& capsule, Contact* contact = 0);

	bool SphereToAabb( const Sphere& sphere, const Aabb& aabb, Contact* contact = 0);

	bool SphereToObb( const Sphere& sphere, const Obb& obb, Contact* contact = 0);

	bool SphereToTriangle( const Sphere& sphere, const Float3& a, const Float3& b, const Float3& c, Contact* contact = 0);



	float ClosestPtSphereSolution( const Sphere& s, const Float3& p, Float3& cp );

	bool SphereToSphereSolution( const Sphere& sphere1, const Sphere& sphere2, Contact* contact = 0);

	bool SphereToCapsuleSolution( const Sphere& sphere, const Capsule& capsule, Contact* contact = 0);

	bool SphereToAabbSolution( const Sphere& sphere, const Aabb& aabb, Contact* contact = 0);

	bool SphereToObbSolution( const Sphere& sphere, const Obb& obb, Contact* contact = 0);

	bool SphereToTriangleSolution( const Sphere& sphere, const Float3& a, const Float3& b, const Float3& c, Contact* contact = 0);
}