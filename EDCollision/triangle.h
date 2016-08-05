#pragma once

#include "EDCollision.h"

#include "../EDMath/float3.h"

using namespace EDMath;

namespace EDCollision
{
	float ClosestPtTriangle( const Float3& p, const Float3& a, const Float3& b, const Float3& c, Float3& cp );

	float ClosestPtTriangleSolution( const Float3& p, const Float3& a, const Float3& b, const Float3& c, Float3& cp );
}