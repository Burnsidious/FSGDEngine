#include "precompiled.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"
#include "../EDMath/capsule.h"
#include "../EDCollision/EDCollision.h"
#include "../EDCollision/segment.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDUtilities/InternalOutput.h"

using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDCollision
{
	/***/
	bool CapsuleToCapsuleSolution( const Capsule& capsule1, const Capsule& capsule2, Contact* contact )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Capsule Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::CapsuleToCapsuleSolution");
			added = true;
		}

		float offset = capsule1.height * 0.5f;
		Float3 s1 = capsule1.center - capsule1.direction * offset;
		Float3 s2 = capsule1.center + capsule1.direction * offset;

		offset = capsule2.height * 0.5f;
		Float3 s3 = capsule2.center - capsule2.direction * offset;
		Float3 s4 = capsule2.center + capsule2.direction * offset;

		float r = capsule1.radius + capsule2.radius;

		Float3 c1;
		Float3 c2;

		float sqD = ClosestPtSegmentSegment( s1, s2, s3, s4, c1, c2 );

		if( sqD < r*r )
		{
			if( contact )
			{
				float d = sqrtf( sqD );
				float pd = r - d;

				if( d == 0.0f )
					return false;

				contact->Normal = (c1 - c2) * (1.0f / d);
				contact->PenetrationDepth = pd;
				contact->Point[0] = c1 - contact->Normal * capsule1.radius;
				contact->numPoints = 1;
			}

			return true;
		}

		return false;
	}

	bool CapsuleToAabbSolution( const Capsule& capsule, const Aabb& aabb, Contact* contact )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Capsule Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::CapsuleToAabbSolution");
			added = true;
		}

		Float3 d = capsule.direction * capsule.height * 0.5f;

		Float3 t = capsule.center - aabb.center;
		Float3 s1 = t - d;
		Float3 s2 = t + d;
		Float3 e = aabb.extents;

		float penDepth = FLT_MAX;
		unsigned int penAxis = 0;

		float adx = abs(d.x);
		float projR = adx + capsule.radius;
		float pd = (projR + e.x) - abs(t.x);

		if( pd <= 0.0f )
			return false;

		penDepth = pd;

		float ady = abs(d.y);

		projR = ady + capsule.radius;
		pd = (projR + e.y) - abs(t.y);

		if( pd <= 0.0f )
			return false;

		if( pd < penDepth )
		{
			penDepth = pd;
			penAxis = 1;
		}
		
		float adz = abs(d.z);

		projR = adz + capsule.radius;
		pd = (projR + e.z) - abs(t.z);

		if( pd <= 0.0f )
			return false;

		if( pd < penDepth )
		{
			penDepth = pd;
			penAxis = 2;
		}

		Float3 capep;
		capep.x = capsule.direction.x < 0.0f ? capsule.direction.x - ED_EPSILON : capsule.direction.x + ED_EPSILON;
		capep.y = capsule.direction.y < 0.0f ? capsule.direction.y - ED_EPSILON : capsule.direction.y + ED_EPSILON;
		capep.z = capsule.direction.z < 0.0f ? capsule.direction.z - ED_EPSILON : capsule.direction.z + ED_EPSILON;

		Float3 axes[3] =
		{
			Float3( 0.0f, -capep.z, capep.y ),
			Float3( capep.z, 0.0f, -capep.x ),
			Float3( -capep.y, capep.x, 0.0f )
		};

		for(unsigned int i = 0; i < 3; ++i)
		{
			float mag = axes[i].magnitude();
			float invMag = 1.0f / mag;
			axes[i] *= invMag;

			float rb = e.x * abs(axes[i].x) + e.y * abs(axes[i].y) + e.z * abs(axes[i].z);
			float dist = abs( DotProduct( axes[i], t ) );
			
			pd = (rb + capsule.radius) - dist;

			if( pd <= 0.0f )
				return false;

			if( pd < penDepth )
			{
				penDepth = pd;
				penAxis = 3 + i;
			}
		}

		if( contact )
		{
			if( penAxis < 3 )
			{
				unsigned int clipAxes[3][2] = { {1, 2}, {0, 2}, {0, 1} };

				for(unsigned int i = 0; i < 2; ++i)
				{
					unsigned int clipAxis = clipAxes[penAxis][i];
					float offset = aabb.extents.v[clipAxis];

					if( s1.v[clipAxis] < offset &&
						s2.v[clipAxis] > offset )
					{
						Float3 delta = s2 - s1;
						s2 = s1 + delta * ((offset - s1.v[clipAxis]) / delta.v[clipAxis]);
					}
					else if( s1.v[clipAxis] > offset &&
						s2.v[clipAxis] < offset )
					{
						Float3 delta = s1 - s2;
						s1 = s2 + delta * ((offset - s2.v[clipAxis]) / delta.v[clipAxis]);
					}
					
					if( s1.v[clipAxis] > -offset &&
						s2.v[clipAxis] < -offset )
					{
						Float3 delta = s2 - s1;
						s2 = s1 + delta * ((-offset - s1.v[clipAxis]) / delta.v[clipAxis]);
					}
					else if( s1.v[clipAxis] < -offset &&
						s2.v[clipAxis] > -offset )
					{
						Float3 delta = s1 - s2;
						s1 = s2 + delta * ((-offset - s2.v[clipAxis]) / delta.v[clipAxis]);
					}
				}

				contact->PenetrationDepth = penDepth;
				contact->Normal.makeZero();
				float scale = ( t.v[penAxis] > 0.0f ? 1.0f : -1.0f );
				contact->Normal.v[penAxis] = scale;
				contact->numPoints = 1;

				float diff = s1.v[penAxis] - s2.v[penAxis];

				if( abs(diff) < ED_EPSILON )
					contact->Point[0] = (s1 + s2) * 0.5f - contact->Normal * capsule.radius + aabb.center;
				else if( diff < 0.0f )
					contact->Point[0] = s1 - contact->Normal * capsule.radius + aabb.center;
				else
					contact->Point[0] = s2 - contact->Normal * capsule.radius + aabb.center;
			}
			else
			{
				penAxis -= 3;
				// penAxis is now index of axis of minimum overlap
				// in axes array AND the index of the axis (X/Y/Z)
				// that is perpendicular to axis of minimum overlap.

				// Make sure axis of minimum overlap points towards capsule.
				// Gives the normal of the surface at point of impact.
				if( DotProduct( t, axes[penAxis] ) < 0.0f )
					axes[penAxis].negate();

				/*BEGIN FINDING CONTACT POINT*/

				// Following code determines exactly which
				// edge of the AABB the capsule collides with.
				// Uses axis of minimum overlap and aabb extents
				// to determine endpoints of the edge-segment.

				// Edge perpendicular to axis of minimum overlap
				// extends from -e.v[penAxis] to e.v[penAxis]
				float penAxisVal = e.v[penAxis];

				//__declspec(align(16)) float a16[] = { axes[penAxis].x, axes[penAxis].y, axes[penAxis].z, 1.0f };
				//__declspec(align(16)) float e16[] = { e.x, e.y, e.z, 1.0f };
				//__declspec(align(16)) float r16[] = {0.0f, 0.0f, 0.0f, 0.0f};

				//_mm_store_ps( r16, _mm_div_ps( _mm_load_ps(a16), _mm_load_ps(e16) ) );

				// Scale the axis into local space of unit aabb
				Float3 scaledAxis( axes[penAxis].x / e.x, axes[penAxis].y / e.y, axes[penAxis].z / e.z );
				//Float3 scaledAxis( r16[0], r16[1], r16[2] );

				// Scaled axis component with largest absolute value
				// gives the axis most aligned with original value
				unsigned int maxAxis = 0;
				if( abs(scaledAxis.y) > abs(scaledAxis.x) )
					maxAxis = 1;
				if( abs(scaledAxis.z) > abs(scaledAxis.v[maxAxis]) )
					maxAxis = 2;

				// Sign of component gives direction of
				// most aligned axis, ex: -X or +X
				float maxAxisVal = scaledAxis.v[maxAxis] < 0.0f ? -e.v[maxAxis] : e.v[maxAxis];

				// penAxis and maxAxis will be 0, 1, or 2 and not equal.
				// thirdAxis should be 0, 1, or 2 and not equal.
				// Given axes(0,1) - 2 is missing, 0|1 == 1
				// Given axes(0,2) - 1 is missing, 0|2 == 2
				// Given axes(1,2) - 0 is missing, 1|2 == 3
				// So missing axis is...
				unsigned int thirdAxis = 3 - (penAxis | maxAxis);

				// If the axis of overlap points up/forward/right (+)
				// the edge is on the positive side. Otherwise on negative side.
				float thirdVal = axes[penAxis].v[thirdAxis] < 0.0f ? -e.v[thirdAxis] : e.v[thirdAxis];

				Float3 p1;
				p1.v[maxAxis] = maxAxisVal;
				p1.v[penAxis] = penAxisVal;
				p1.v[thirdAxis] = thirdVal;

				Float3 q1;
				q1.v[maxAxis] = maxAxisVal;
				q1.v[penAxis] = -penAxisVal;
				q1.v[thirdAxis] = thirdVal;

				Float3 cpAabb;
				Float3 cpCap;

				ClosestPtSegmentSegment( s1, s2, p1, q1, cpCap, cpAabb );

				contact->Normal = axes[penAxis];
				contact->PenetrationDepth = penDepth;
				contact->Point[0] = (cpCap - axes[penAxis] * capsule.radius) + aabb.center;
				contact->numPoints = 1;

				/*END FINDING CONTACT POINT*/
			}
		}

		return true;
	}

	//bool CapsuleToAabbSolution( const Capsule& capsule, const Aabb& aabb, Contact* contact )
	//{
	//	static bool added = false;
	//	if(!added)
	//	{
	//		PrintConsole::GetReference().AddSolution(L"ED3", "Capsule Collision", Float3(1.0f, 0.0f, 0.0f), 
	//			L"EDCollision::CapsuleToAabbSolution");
	//		added = true;
	//	}

	//	float offset = capsule.height * 0.5f;
	//	Float3 s1 = capsule.center - capsule.direction * offset;
	//	Float3 s2 = capsule.center + capsule.direction * offset;
	//	
	//	s1 -= aabb.center;
	//	s2 -= aabb.center;

	//	Float3 c1;
	//	Float3 c2;
	//	
	//	float sqD = ClosestPtSegmentAabb( s1, s2, aabb.extents, c1, c2 );

	//	if( sqD < capsule.radius * capsule.radius )
	//	{
	//		if( contact )
	//		{
	//			c1 += aabb.center;
	//			c2 += aabb.center;

	//			float d = sqrtf(sqD);
	//			float pd = capsule.radius - d;

	//			if( d == 0.0f || pd < 0.001f )
	//				return false;

	//			contact->Normal = (c1 - c2) * (1.0f/d);
	//			contact->PenetrationDepth = pd;
	//			contact->Point[0] = c1;
	//			contact->numPoints = 1;
	//		}

	//		return true;
	//	}

	//	return false;
	//}

	bool CapsuleToObbSolution( const Capsule& capsule, const Obb& obb, Contact* contact )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Capsule Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::CapsuleToObbSolution");
			added = true;
		}

		Capsule oCapsule = capsule;
		
		InvTransformPoint( oCapsule.center, capsule.center, obb.transform );
		InvTransformVector( oCapsule.direction, capsule.direction, obb.transform );

		Aabb oBox;
		oBox.center.makeZero();
		oBox.extents = obb.extents;

		if( CapsuleToAabbSolution( oCapsule, oBox, contact ) )
		{
			if( contact )
			{
				TransformPoint( contact->Point[0], contact->Point[0], obb.transform );
				TransformVector( contact->Normal, contact->Normal, obb.transform );
			}

			return true;
		}

		return false;
	}

	bool CapsuleToTriangleSolution( const Capsule& capsule, const Float3& a, const Float3& b, const Float3& c, Contact* contact )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Capsule Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::CapsuleToTriangleSolution");
			added = true;
		}

		float offset = capsule.height * 0.5f;
		Float3 s1 = capsule.center - capsule.direction * offset;
		Float3 s2 = capsule.center + capsule.direction * offset;

		Float3 c1;
		Float3 c2;

		float sqD = ClosestPtSegmentTriangle( s1, s2, a, b, c, c1, c2 );

		if( sqD < capsule.radius * capsule.radius )
		{
			if( contact )
			{
				float d = sqrtf( sqD );
				float pd = capsule.radius - d;

				if( d == 0.0f )
				{
					CrossProduct( contact->Normal, b - a, c - b );
					contact->Normal.normalize();

					float d = DotProduct( s2 - c1, contact->Normal );
					pd = capsule.radius - d;
					c1 = s2;
				}
				else
				{
					contact->Normal = (c1 - c2) * (1.0f / d);
				}

				contact->Point[0] = c1 - contact->Normal * capsule.radius;
				contact->PenetrationDepth = pd;
				contact->numPoints = 1;
			}

			return true;
		}

		return false;
	}
}