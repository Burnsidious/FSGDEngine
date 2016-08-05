#include "segment.h"
#include "EDCollision.h"
#include "ray.h"

namespace EDCollision
{
	float SquaredDistance( const Float3& p, const Float3& s1, const Float3& s2 )
	{
		return SquaredDistanceSolution( p, s1, s2 );
	}

	float ClosestPtSegmentSegment( const Segment& s1, const Segment& s2, Float3 &c1, Float3 &c2 )
	{
		return ClosestPtSegmentSegmentSolution( s1, s2, c1, c2 );
	}

	float ClosestPtSegmentSegment( const Float3 &p1, const Float3 &q1, const Float3 &p2, const Float3 &q2, Float3 &c1, Float3 &c2 )
	{
		return ClosestPtSegmentSegmentSolution( p1, q1, p2, q2, c1, c2 );
	}

	float ClosestPtSegment( const Float3 &p, const Segment& s, Float3& c )
	{
		return ClosestPtSegmentSolution( p, s, c );
	}

	float ClosestPtSegment( const Float3 &p, const Float3& s1, const Float3& s2, Float3& c )
	{
		return ClosestPtSegmentSolution( p, s1, s2, c );
	}

	float ClosestPtSegmentTriangle( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c, Float3& cp1, Float3& cp2 )
	{
		return ClosestPtSegmentTriangleSolution( p, q, a, b, c, cp1, cp2 );
	}

	float ClosestPtSegmentAabb( const Float3& p, const Float3& q, const Float3& e, Float3& c1, Float3& c2 )
	{
		return ClosestPtSegmentAabbSolution( p, q, e, c1, c2 );
	}

	float ClosestPtSegmentPlane( const Float3& p, const Float3& q, const Float3& n, float offset, Float3& cp1, Float3& cp2  )
	{
		return ClosestPtSegmentPlaneSolution( p, q, n, offset, cp1, cp2 );
	}

	float ClosestPtSegmentPlane( const Float3& p, const Float3& q, const Float3& n, const Float3& a, Float3& cp1, Float3& cp2 )
	{
		return ClosestPtSegmentPlaneSolution( p, q, n, a, cp1, cp2 );
	}

	bool SegmentToSphere( const Float3& p, const Float3& q, const Sphere& sphere )
	{
		return SegmentToSphereSolution( p, q, sphere );
	}

	bool SegmentToSphere( const Float3& p, const Float3& q, const Sphere& sphere, Contact& contact )
	{
		return SegmentToSphereSolution( p, q, sphere, contact );
	}

	bool SegmentToCapsule( const Float3& p, const Float3& q, const Capsule& capsule )
	{
		return SegmentToCapsuleSolution( p, q, capsule );
	}

	bool SegmentToCapsule( const Float3& p, const Float3& q, const Capsule& capsule, Contact& contact )
	{
		return SegmentToCapsuleSolution( p, q, capsule, contact );
	}

	bool SegmentToAabb( const Float3& p, const Float3& q, const Aabb& aabb )
	{
		return SegmentToAabbSolution( p, q, aabb );
	}

	bool SegmentToObb( const Float3& p, const Float3& q, const Obb& obb )
	{
		return SegmentToObbSolution( p, q, obb );
	}

	bool SegmentToTriangle( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c )
	{
		return SegmentToTriangleSolution( p, q, a, b, c );
	}

	bool SegmentToTriangle( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c, Float3& cp )
	{
		return SegmentToTriangleSolution( p, q, a, b, c, cp );
	}

	bool SegmentToPlane( const Float3& p, const Float3& q, const Float3& n, float offset )
	{
		return SegmentToPlaneSolution( p, q, n, offset );
	}

	bool SegmentToPlane( const Float3& p, const Float3& q, const Float3& n, float offset, Float3& cp )
	{
		return SegmentToPlaneSolution( p, q, n, offset, cp );
	}
}