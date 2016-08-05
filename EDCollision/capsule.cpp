#include "capsule.h"

#include "EDCollision.h"
#include "segment.h"

namespace EDCollision
{
	bool CapsuleToCapsule( const Capsule& capsule1, const Capsule& capsule2, Contact* contact )
	{
		return CapsuleToCapsuleSolution( capsule1, capsule2, contact );
	}

	bool CapsuleToAabb( const Capsule& capsule, const Aabb& aabb, Contact* contact )
	{
		return CapsuleToAabbSolution(capsule, aabb, contact);
	}

	bool CapsuleToObb( const Capsule& capsule, const Obb& obb, Contact* contact )
	{
		return CapsuleToObbSolution( capsule, obb, contact );
	}

	bool CapsuleToTriangle( const Capsule& capsule, const Float3& a, const Float3& b, const Float3& c, Contact* contact )
	{
		return CapsuleToTriangleSolution( capsule, a, b, c, contact );
	}
}