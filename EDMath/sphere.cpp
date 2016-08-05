#include "sphere.h"
#include "aabb.h"

#include <assert.h>

namespace EDMath
{
	Aabb Sphere::GetAabb(void)const
	{
		return Aabb( center, Float3(radius, radius, radius) );
	}

	void ComputeSphere(Sphere &sphere, const Float3 pts[], unsigned int numPts)
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

		sphere.center = (vmin + vmax) * 0.5f;
		sphere.radius = 0.0f;
	
		for(unsigned int i = 0; i < numPts; ++i)
		{
			Float3 delta = pts[i] - sphere.center;
			float dsq = DotProduct( delta, delta );

			if( sphere.radius < dsq )
				sphere.radius = dsq;
		}

		sphere.radius = sqrtf(sphere.radius);
	}

	void ComputeSphere(Sphere &sphere, const float* pts, unsigned int numPts, size_t stride)
	{
		assert( numPts != 0 );

		Float3 vmin( pts[0], pts[1], pts[2] );
		Float3 vmax( pts[0], pts[1], pts[2] );

		for(unsigned int i = 1; i < numPts; ++i)
		{
			const float* pt = (float*)(((const char*)pts) + i * stride);
			for(unsigned int axis = 0; axis < 3; ++axis)
			{
				if( vmin.v[axis] > pt[axis] )
					vmin.v[axis] = pt[axis];
				if( vmax.v[axis] < pt[axis] )
					vmax.v[axis] = pt[axis];
			}
		}

		sphere.center = (vmin + vmax) * 0.5f;
		sphere.radius = 0.0f;
	
		for(unsigned int i = 0; i < numPts; ++i)
		{
			const float* pt = (float*)(((const char*)pts) + i * stride);

			Float3 delta( pt[0], pt[1], pt[2] );
			delta -= sphere.center;

			float dsq = DotProduct( delta, delta );

			if( sphere.radius < dsq )
				sphere.radius = dsq;
		}

		sphere.radius = sqrtf(sphere.radius);

	}

	void BuildSphere(Sphere& out, const Sphere& a, const Sphere& b)
	{
		Float3 d = b.center - a.center;
		float distSq = d.squaredMagnitude();

		if( (b.radius - a.radius)*(b.radius - a.radius) >= distSq )
		{
			if( b.radius >= a.radius )
				out = b;
			else
				out = a;
		}
		else
		{
			float dist = sqrtf(distSq);
			out.radius = (dist + a.radius + b.radius) * 0.5f;
			out.center = a.center;
			if( dist > 0.001f )
				out.center += d * ((out.radius - a.radius) / dist);
		}
	}

	void TransformSphere(Sphere &out, const Sphere& in, const Float4x4 transform)
	{
		TransformPoint( out.center, in.center, transform );
	}
}