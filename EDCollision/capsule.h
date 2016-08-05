#pragma once

#include "EDCollision.h"

#include "../EDMath/capsule.h"
#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"

namespace EDCollision
{
	bool CapsuleToCapsule( const Capsule& capsule1, const Capsule& capsule2, Contact* contact = 0 );
	bool CapsuleToCapsuleSolution( const Capsule& capsule1, const Capsule& capsule2, Contact* contact = 0 );

	bool CapsuleToAabb( const Capsule& capsule, const Aabb& aabb, Contact* contact = 0 );
	bool CapsuleToAabbSolution( const Capsule& capsule, const Aabb& aabb, Contact* contact = 0 );

	bool CapsuleToObb( const Capsule& capsule, const Obb& obb, Contact* contact = 0 );
	bool CapsuleToObbSolution( const Capsule& capsule, const Obb& obb, Contact* contact = 0 );

	bool CapsuleToTriangle( const Capsule& capsule, const Float3& a, const Float3& b, const Float3& c, Contact* contact = 0 );
	bool CapsuleToTriangleSolution( const Capsule& capsule, const Float3& a, const Float3& b, const Float3& c, Contact* contact = 0 );
}