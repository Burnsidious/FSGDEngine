#include "aabb.h"
#include <assert.h>

namespace EDMath
{
	void ComputeAabb(Aabb& aabb, const Float3 pts[], unsigned int numPts)
	{
		assert( numPts != 0 );

		Float3 vmin = pts[0];
		Float3 vmax = pts[0];

		for(unsigned int i = 1; i < numPts; ++i)
		{
			for(unsigned int axis = 0; axis < 3; ++axis)
			{
				if( vmin.v[axis] > pts[i].v[axis] )
					vmin.v[axis] = pts[i].v[axis];
				if( vmax.v[axis] < pts[i].v[axis] )
					vmax.v[axis] = pts[i].v[axis];
			}
		}

		aabb.center = (vmin + vmax) * 0.5f;
		aabb.extents = vmax - aabb.center;
	}

	void ComputeAabb(Aabb& aabb, const float* pts, unsigned int numPts, size_t offset)
	{
		Float3 vmin( pts[0], pts[1], pts[2] );
		Float3 vmax( pts[0], pts[1], pts[2] );

		for(unsigned int i = 1; i < numPts; ++i)
		{
			size_t byteOffset = i * offset;
			const float* point = (const float*)(((const char*)pts) + byteOffset);
			for(unsigned int axis = 0; axis < 3; ++axis)
			{
				if( vmin.v[axis] > point[axis] )
					vmin.v[axis] = point[axis];
				if( vmax.v[axis] < point[axis] )
					vmax.v[axis] = point[axis];
			}
		}

		aabb.center = (vmin + vmax) * 0.5f;
		aabb.extents = vmax - aabb.center;
	}

	void TransformAabb(Aabb& out, const Aabb& in, const Float4x4& transform)
	{
		Aabb temp;

		for(int i = 0; i < 3; ++i)
		{
			temp.center.v[i] = transform.WAxis.v[i];
			temp.extents.v[i] = 0.0f;

			for(int j = 0; j < 3; ++j)
			{
				temp.center.v[i] += transform.m[j][i] * in.center.v[j];
				temp.extents.v[i] += abs(transform.m[j][i]) * in.extents.v[j];
			}
		}

		out = temp;
	}
}