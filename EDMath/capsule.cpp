#include "capsule.h"
#include "aabb.h"

namespace EDMath
{
	Aabb Capsule::GetAabb(void)const
	{
		float offset = height * 0.5f;

		return Aabb( center, Float3( abs(direction.x) * offset + radius, abs(direction.y) * offset + radius, abs(direction.z) * offset + radius ) );
	}

	void ComputeCapsule(Capsule& capsule, const Float3 pts[], unsigned int numPts)
	{
		Aabb aabb;
		ComputeAabb( aabb, pts, numPts );

		int axes[2] = { 0, 1 };

		if( aabb.extents.x >= aabb.extents.z && aabb.extents.y >= aabb.extents.z )
		{
			if( aabb.extents.x < aabb.extents.y )
			{
				axes[0] = 1;
				axes[1] = 0;
			}
		}
		else if( aabb.extents.y >= aabb.extents.x && aabb.extents.z >= aabb.extents.x )
		{
			if( aabb.extents.y > aabb.extents.z )
			{
				axes[0] = 1;
				axes[1] = 2;
			}
			else
			{
				axes[0] = 2;
				axes[1] = 1;
			}
		}
		else if( aabb.extents.x >= aabb.extents.y && aabb.extents.z >= aabb.extents.y )
		{
			if( aabb.extents.x < aabb.extents.z )
			{
				axes[0] = 2;
				axes[1] = 0;
			}
			else
				axes[1] = 2;
		}

		capsule.center = aabb.center;
		capsule.direction.makeZero();
		capsule.direction.v[ axes[0] ] = 1.0f;
		capsule.radius = aabb.extents.v[ axes[1] ];
		capsule.height = (aabb.extents.v[ axes[0] ] + capsule.radius) * 2.0f;
	}

	void TransformCapsule(Capsule& out, const Capsule& in, const Float4x4& transform)
	{
		TransformPoint( out.center, in.center, transform );
		TransformVector( out.direction, in.direction, transform );
	}
}