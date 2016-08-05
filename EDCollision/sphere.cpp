#include "sphere.h"

#include "EDCollision.h"
#include "segment.h"
#include "triangle.h"

namespace EDCollision
{
	float ClosestPtSphere( const Sphere& s, const Float3& p, Float3& cp )
	{
		return ClosestPtSphereSolution( s, p, cp );
	}

	bool SphereToSphere( const Sphere& sphere1, const Sphere& sphere2, Contact* contact )
	{
		return SphereToSphereSolution( sphere1, sphere2, contact );
	}

	bool SphereToCapsule( const Sphere& sphere, const Capsule& capsule, Contact* contact )
	{
		return SphereToCapsuleSolution( sphere, capsule, contact );
	}

	bool SphereToAabb( const Sphere& sphere, const Aabb& aabb, Contact* contact )
	{
		return SphereToAabbSolution( sphere, aabb, contact );
	}

	bool SphereToTriangle( const Sphere& sphere, const Float3& a, const Float3& b, const Float3& c, Contact* contact )
	{
		return SphereToTriangleSolution( sphere, a, b, c, contact );
	}

	bool SphereToObb( const Sphere& sphere, const Obb& obb, Contact* contact  )
	{
		return SphereToObbSolution( sphere, obb, contact );
	}
}