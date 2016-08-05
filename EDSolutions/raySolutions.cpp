#include "precompiled.h"
#include "../EDUtilities/PrintConsole.h"

#include "../EDCollision/EDCollision.h"
#include "../EDMath/sphere.h"
#include "../EDMath/capsule.h"
#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"
#include "../EDCollision/segment.h"

using EDUtilities::PrintConsole;
using EDMath::Float3;

#ifndef SameSign
#define SameSign(a,b) ( (((unsigned int&)(a)) & 0x80000000) == (((unsigned int&)(b)) & 0x80000000) )
#endif 

namespace EDCollision
{
	bool OverlapRaySphereSolution(Float3 start, Float3 direction, float maxLength, const Sphere& sphere)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::OverlapRaySphereSolution");
			added = true;
		}

		Float3 v = sphere.center - start;

		if( v.squaredMagnitude() < sphere.radius * sphere.radius )
			return false;

		float d = DotProduct( v, direction );

		if( d < 0.0f )
			d = 0.0f;
		if( d > maxLength )
			d = maxLength;

		Float3 cp = start + direction * d;
		v = cp - sphere.center;

		return v.squaredMagnitude() < sphere.radius * sphere.radius;
	}

	bool IntersectRaySphereSolution(Float3 start, Float3 direction, float maxLength, const Sphere& sphere, Float3& cp, Float3& cn)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::IntersectRaySphereSolution");
			added = true;
		}

		Float3 m = start - sphere.center;
		float c = DotProduct(m, m) - sphere.radius*sphere.radius;
		float b = DotProduct(m, direction);
		
		if( c > 0 && b > 0 )
			return false;

		float disc = b*b - c;

		if( disc < 0.0f )
			return false;

		float t = -b - sqrt(disc);

		if( t < 0.0f || t > maxLength )
			return false;

		cp = start + direction * t;
		cn = (cp - sphere.center).normalize();

		return true;
	}

	bool OverlapRayCylinderSolution(const Float3 &start, const Float3 &direction, float maxLength, const Float3 &p, const Float3 &q, float r)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::OverlapRayCylinderSolution");
			added = true;
		}

		Float3 d = q - p, m = start - p;

		float dd = DotProduct( d, d );
		float nd = DotProduct( direction, d );
		float mn = DotProduct( m, direction );
		float md = DotProduct( m, d );
		float mm = DotProduct( m, m );

		// TODO: Optimization
		if( md < 0.0f && nd < 0.0f )
			return false;
		if( md > dd && nd > 0.0f )
			return false;

		float a = dd - nd * nd;
		float b = dd*mn - nd*md;
		float c = dd*(mm - r*r) - md*md;

		// If a is approximately 0.0 then the ray is parallel to the cylinder and can't intersect
		if( abs(a) < FLT_EPSILON )
			return false;

		// TODO: Find time of intersection...
		float discr = b*b - a*c;

		if( discr < 0.0f )
			return false;

		float t = (-b -sqrt(discr)) / a;

		if( t < 0.0f || t > maxLength )
			return false;

		float projDist = md + t * nd;

		if( projDist > dd || projDist < 0.0f )
			return false;

		return true;
	}

	bool IntersectRayCylinderSolution(const Float3 &start, const Float3 &direction, float maxLength, const Float3 &p, const Float3 &q, float r, Float3& cp, Float3& cn)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::IntersectRayCylinderSolution");
			added = true;
		}

		Float3 d = q - p, m = start - p;

		float dd = DotProduct( d, d );
		float nd = DotProduct( direction, d );
		float mn = DotProduct( m, direction );
		float md = DotProduct( m, d );
		float mm = DotProduct( m, m );

		// TODO: Optimization
		if( md < 0.0f && nd < 0.0f )
			return false;
		if( md > dd && nd > 0.0f )
			return false;

		float a = dd - nd * nd;
		float b = dd*mn - nd*md;
		float c = dd*(mm - r*r) - md*md;

		// If a is approximately 0.0 then the ray is parallel to the cylinder and can't intersect
		if( abs(a) < FLT_EPSILON )
			return false;

		// TODO: Find time of intersection...
		float discr = b*b - a*c;

		if( discr < 0.0f )
			return false;

		float t = (-b -sqrt(discr)) / a;

		if( t < 0.0f || t > maxLength )
			return false;

		float projDist = md + t * nd;

		if( projDist > dd || projDist < 0.0f )
			return false;

		cp = start + direction * t;

		Float3 v = cp - p;
		float ratio = DotProduct( v, d ) / dd;
		v = p + d * ratio;

		cn = (cp - v).normalize();

		return true;
	}

	bool OverlapRayCapsuleSolution(Float3 start, Float3 direction, float maxLength, const Capsule& capsule)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::OverlapRayCapsuleSolution");
			added = true;
		}

		float offset = capsule.height * 0.5f;
		Float3 s1 = capsule.center - capsule.direction * offset;
		Float3 s2 = capsule.center + capsule.direction * offset;
		
		if( SquaredDistance( start, s1, s2 ) > capsule.radius*capsule.radius )
		{
			if( OverlapRayCylinderSolution( start, direction, maxLength, s1, s2, capsule.radius ) )
				return true;

			Sphere s;
			s.center = s1;
			s.radius = capsule.radius;
			if( OverlapRaySphereSolution( start, direction, maxLength, s ) )
				return true;

			s.center = s2;
			if( OverlapRaySphereSolution( start, direction, maxLength, s ) )
				return true;
		}
		
		return false;
	}

	bool IntersectRayCapsuleSolution(Float3 start, Float3 direction, float maxLength, const Capsule& capsule, Float3& cp, Float3& cn)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::IntersectRayCapsuleSolution");
			added = true;
		}

		float offset = capsule.height * 0.5f;
		Float3 s1 = capsule.center - capsule.direction * offset;
		Float3 s2 = capsule.center + capsule.direction * offset;
		
		if( SquaredDistance( start, s1, s2 ) > capsule.radius*capsule.radius )
		{
			if( IntersectRayCylinderSolution( start, direction, maxLength, s1, s2, capsule.radius, cp, cn ) )
				return true;

			float t = maxLength;
			Sphere sphere1;
			sphere1.center = s1;
			sphere1.radius = capsule.radius;

			if( IntersectRaySphereSolution( start, direction, maxLength, sphere1, cp, cn ) )
				t = DotProduct( cp - start, direction );

			Sphere sphere2;
			sphere2.center = s2;
			sphere2.radius = capsule.radius;
			if( IntersectRaySphereSolution( start, direction, t, sphere2, cp, cn ) )
				return true;

			return (t != maxLength);
		}

		return false;
	}

	bool OverlapRayAabbSolution(Float3 start, Float3 direction, float maxLength, const Aabb& aabb)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::OverlapRayAabbSolution");
			added = true;
		}

		float tmin = 0.0f;
		float tmax = FLT_MAX;

		Float3 vMin = aabb.Min();
		Float3 vMax = aabb.Max();

		for(int i = 0; i < 3; ++i)
		{
			if( abs( direction.v[i] ) < ED_EPSILON )
			{
				if( start.v[i] < vMin.v[i] || start.v[i] > vMax.v[i] )
					return false;
			}
			else
			{
				float ood = 1.0f / direction.v[i];
				float t1 = (vMin.v[i] - start.v[i]) * ood;
				float t2 = (vMax.v[i] - start.v[i]) * ood;

				if( t1 > t2 )
					std::swap(t1, t2);

				tmin = max(tmin, t1);
				tmax = min(tmax, t2);

				if( tmin > tmax )
					return false;
			}
		}

		if( tmin > maxLength )
			return false;

		return true;
	}

	bool IntersectRayAabbSolution(Float3 start, Float3 direction, float maxLength, const Aabb& aabb, Float3& cp, Float3& cn)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::IntersectRayAabbSolution");
			added = true;
		}

		float tmin = 0.0f;
		float tmax = FLT_MAX;

		Float3 vMin = aabb.Min();
		Float3 vMax = aabb.Max();

		for(int i = 0; i < 3; ++i)
		{
			if( abs( direction.v[i] ) < ED_EPSILON )
			{
				if( start.v[i] < vMin.v[i] || start.v[i] > vMax.v[i] )
					return false;
			}
			else
			{
				float ood = 1.0f / direction.v[i];
				float t1 = (vMin.v[i] - start.v[i]) * ood;
				float t2 = (vMax.v[i] - start.v[i]) * ood;

				if( t1 > t2 )
					std::swap(t1, t2);

				tmin = max(tmin, t1);
				tmax = min(tmax, t2);

				if( tmin > tmax )
					return false;
			}
		}

		if( tmin <= 0.0f || tmin > maxLength )
			return false;

		cp = start + direction * tmin;

		Float3 v = cp - aabb.center;
		v.x /= aabb.extents.x;
		v.y /= aabb.extents.y;
		v.z /= aabb.extents.z;

		int i = 0;
		if( abs( v.y ) > abs(v.x) )
			i = 1;
		if( abs( v.z ) > abs(v.v[i]) )
			i = 2;

		cn.makeZero();
		cn.v[i] = ( v.v[i] < 0.0f ) ? 1.0f : -1.0f;

		return true;
	}

	bool OverlapRayObbSolution(Float3 start, Float3 direction, float maxLength, const Obb& obb)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::OverlapRayObbSolution");
			added = true;
		}

		Float3 p;
		InvTransformPoint( p, start, obb.transform );

		Float3 d;
		InvTransformVector( d, direction, obb.transform );

		Aabb temp;
		temp.center.makeZero();
		temp.extents = obb.extents;

		return OverlapRayAabbSolution( p, d, maxLength, temp );
	}

	bool IntersectRayObbSolution(Float3 start, Float3 direction, float maxLength, const Obb& obb, Float3& cp, Float3& cn)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::IntersectRayObbSolution");
			added = true;
		}

		Float3 p;
		InvTransformPoint( p, start, obb.transform );

		Float3 d;
		InvTransformVector( d, direction, obb.transform );

		Aabb temp;
		temp.center.makeZero();
		temp.extents = obb.extents;

		Float3 contact;
		Float3 normal;
		if( IntersectRayAabbSolution( p, d, maxLength, temp, contact, normal  ) )
		{
			TransformPoint( cp, contact, obb.transform );
			TransformVector( cn, normal, obb.transform );
			return true;
		}

		return false;
	}

	bool IntersectRayTriangleSolution(Float3 start, Float3 direction, float maxLength, Float3 a, Float3 b, Float3 c, Float3& bcCoords, float& t)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Ray Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::IntersectRayTriangleSolution");
			added = true;
		}

		Float3 pq = direction;
		Float3 pa = a - start;
		Float3 pb = b - start;
		Float3 pc = c - start;

		Float3 cross;
		CrossProduct( cross, pc, pb );
		float u = DotProduct( pq, cross );

		CrossProduct( cross, pa, pc );
		float v = DotProduct( pq, cross );

		if( !SameSign( u, v ) )
			return false;

		CrossProduct( cross, pb, pa );
		float w = DotProduct( pq, cross );

		if( !SameSign( u, w ) )
			return false;

		if( abs(u) < FLT_EPSILON && abs(v) < FLT_EPSILON && abs(w) < FLT_EPSILON )
			return false;

		float denom = 1.0f / (u + v + w);
		
		u *= denom;
		v *= denom;
		w *= denom;

		Float3 pt( a*u + b*v + c*w );

		float dist = DotProduct( pt - start, direction );
		if( dist > 0.0f && dist < maxLength )
		{
			t = dist;
			bcCoords.x = u;
			bcCoords.y = v;
			bcCoords.z = w;
			return true;
		}

		return false;
	}
}