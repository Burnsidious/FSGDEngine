#include "triangle.h"
#include "EDCollision.h"

namespace EDCollision
{
	float ClosestPtTriangle( const Float3& p, const Float3& a, const Float3& b, const Float3& c, Float3& closest )
	{
		return ClosestPtTriangleSolution(p, a, b, c, closest );
	}
}
