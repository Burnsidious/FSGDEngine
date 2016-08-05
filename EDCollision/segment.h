#pragma once

#include "EDCollision.h"
#include "../EDMath/segment.h"
#include "../EDMath/plane.h"
#include "../EDMath/sphere.h"
#include "../EDMath/capsule.h"
#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"

using namespace EDMath;

namespace EDCollision
{
	float SquaredDistance( const Float3& p, const Float3& s1, const Float3& s2 );

	float ClosestPtSegmentSegment( const Segment& s1, const Segment& s2, Float3 &c1, Float3 &c2 );

	float ClosestPtSegmentSegment( const Float3 &p1, const Float3 &q1, const Float3 &p2, const Float3 &q2, Float3 &c1, Float3 &c2 );

	float ClosestPtSegment( const Float3 &p, const Segment& s, Float3& c );

	float ClosestPtSegment( const Float3 &p, const Float3& s1, const Float3& s2, Float3& c );

	float ClosestPtSegmentTriangle( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c, Float3& cp1, Float3& cp2 );

	float ClosestPtSegmentAabb( const Float3& p, const Float3& q, const Float3& e, Float3& c1, Float3& c2 );

	float ClosestPtSegmentPlane( const Float3& p, const Float3& q, const Float3& n, float offset, Float3& cp1, Float3& cp2 );

	float ClosestPtSegmentPlane( const Float3& p, const Float3& q, const Float3& n, const Float3& a, Float3& cp1, Float3& cp2 );

	bool SegmentToSphere( const Float3& p, const Float3& q, const Sphere& sphere );

	bool SegmentToSphere( const Float3& p, const Float3& q, const Sphere& sphere, Contact& contact );

	bool SegmentToCapsule( const Float3& p, const Float3& q, const Capsule& capsule );

	bool SegmentToCapsule( const Float3& p, const Float3& q, const Capsule& capsule, Contact& contact );

	bool SegmentToAabb( const Float3& p, const Float3& q, const Aabb& aabb );

	bool SegmentToObb( const Float3& p, const Float3& q, const Obb& obb );

	bool SegmentToTriangle( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c );

	bool SegmentToTriangle( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c, Float3& cp );

	bool SegmentToPlane( const Float3& p, const Float3& q, const Float3& n, float offset );

	bool SegmentToPlane( const Float3& p, const Float3& q, const Float3& n, float offset, Float3& cp );



	float SquaredDistanceSolution( const Float3& p, const Float3& s1, const Float3& s2 );

	float ClosestPtSegmentSegmentSolution( const Segment& s1, const Segment& s2, Float3 &c1, Float3 &c2 );

	float ClosestPtSegmentSegmentSolution( const Float3 &p1, const Float3 &q1, const Float3 &p2, const Float3 &q2, Float3 &c1, Float3 &c2 );

	float ClosestPtSegmentSolution( const Float3 &p, const Segment& s, Float3& c );

	float ClosestPtSegmentSolution( const Float3 &p, const Float3& s1, const Float3& s2, Float3& c );

	float ClosestPtSegmentTriangleSolution( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c, Float3& cp1, Float3& cp2 );

	float ClosestPtSegmentAabbSolution( const Float3& p, const Float3& q, const Float3& e, Float3& c1, Float3& c2 );

	float ClosestPtSegmentPlaneSolution( const Float3& p, const Float3& q, const Float3& n, float offset, Float3& cp1, Float3& cp2 );

	float ClosestPtSegmentPlaneSolution( const Float3& p, const Float3& q, const Float3& n, const Float3& a, Float3& cp1, Float3& cp2 );

	bool SegmentToSphereSolution( const Float3& p, const Float3& q, const Sphere& sphere );

	bool SegmentToSphereSolution( const Float3& p, const Float3& q, const Sphere& sphere, Contact& contact );

	bool SegmentToCapsuleSolution( const Float3& p, const Float3& q, const Capsule& capsule );

	bool SegmentToCapsuleSolution( const Float3& p, const Float3& q, const Capsule& capsule, Contact& contact );

	bool SegmentToAabbSolution( const Float3& p, const Float3& q, const Aabb& aabb );

	bool SegmentToObbSolution( const Float3& p, const Float3& q, const Obb& obb );

	bool SegmentToTriangleSolution( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c );

	bool SegmentToTriangleSolution( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c, Float3& cp );

	bool SegmentToPlaneSolution( const Float3& p, const Float3& q, const Float3& n, float offset );

	bool SegmentToPlaneSolution( const Float3& p, const Float3& q, const Float3& n, float offset, Float3& cp );
}