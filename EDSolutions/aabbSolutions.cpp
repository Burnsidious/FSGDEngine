#include "precompiled.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"
#include "../EDCollision/EDCollision.h"
using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDCollision
{
	float ClosestPtAabbSolution( const Float3& p, const Aabb& aabb, Float3& cp )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "AABB Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtAabbSolution");
			added = true;
		}

		cp.x = Clamp( p.x, aabb.center.x - aabb.extents.x, aabb.center.x + aabb.extents.x );
		cp.y = Clamp( p.y, aabb.center.y - aabb.extents.y, aabb.center.y + aabb.extents.y );
		cp.z = Clamp( p.z, aabb.center.z - aabb.extents.z, aabb.center.z + aabb.extents.z );

		return (cp - p).squaredMagnitude();
	}

	/***/
	bool AabbToAabbSolution(const Aabb& aabb1, const Aabb& aabb2, Contact* contact)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "AABB Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::AabbToAabbSolution");
			added = true;
		}

		int bestAxis = 0;
		float smallestOverlap = FLT_MAX;

		Float3 t = aabb1.center - aabb2.center;

		for(int i = 0; i < 3; ++i)
		{
			float pd = (aabb1.extents.v[i] + aabb2.extents.v[i]) - abs(t.v[i]);

			if( pd > 0.001f )
			{
				if( pd < smallestOverlap )
				{
					bestAxis = i;
					smallestOverlap = pd;
				}
				
				continue;
			}

			return false;
		}

		if( contact )
		{
			contact->Normal.makeZero();
			contact->Normal.v[bestAxis] = (t.v[bestAxis] < 0.0f ? -1.0f : 1.0f );
			contact->PenetrationDepth = smallestOverlap;
		
			for(int i = 0; i < 3; ++i)
			{
				contact->Point[0].v[i] = Clamp( aabb1.center.v[i], 
											min( aabb1.center.v[i] + aabb1.extents.v[i], aabb2.center.v[i] + aabb2.extents.v[i] ),
											max( aabb1.center.v[i] - aabb1.extents.v[i], aabb2.center.v[i] - aabb2.extents.v[i] ) );
			}

			contact->Point[0].v[bestAxis] = (t.v[bestAxis] < 0.0f ? aabb1.extents.v[bestAxis] : -aabb1.extents.v[bestAxis] );
			contact->numPoints = 1;
		}

		return true;
	}
}