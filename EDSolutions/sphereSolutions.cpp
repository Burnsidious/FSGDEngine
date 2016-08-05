#include "precompiled.h"
#include "../EDUtilities/PrintConsole.h"

#include "../EDMath/sphere.h"

#include "../EDCollision/EDCollision.h"
#include "../EDCollision/segment.h"
#include "../EDCollision/triangle.h"

using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDCollision
{
	float ClosestPtSphereSolution( const Sphere& s, const Float3& p, Float3& cp )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Sphere Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtSphereSolution");
			added = true;
		}

		Float3 v = p - s.center;
		float dSq = v.squaredMagnitude();

		if( dSq < s.radius*s.radius )
		{
			cp = p;
			return dSq;
		}

		v *= 1.0f / sqrtf(dSq);
		cp = s.center + v * s.radius;
		
		return dSq;
	}

	/***/
	bool SphereToSphereSolution( const Sphere& sphere1, const Sphere& sphere2, Contact* contact )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Sphere Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SphereToSphereSolution");
			added = true;
		}

		Float3 v = sphere1.center - sphere2.center;
		float dSq = v.squaredMagnitude();
		float r = sphere1.radius + sphere2.radius;
		
		if( dSq < r*r )
		{
			if( contact )
			{
				float d = sqrtf( dSq );
				float pd = r - d;

				if( d == 0.0f )
					return false;

				contact->Normal = v * (1.0f / d);
				contact->PenetrationDepth = pd;
				contact->Point[0] = sphere1.center - contact->Normal * sphere1.radius;
				contact->numPoints = 1;
			}

			return true;
		}

		return false;
	}

	bool SphereToCapsuleSolution( const Sphere& sphere, const Capsule& capsule, Contact* contact )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Sphere Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SphereToCapsuleSolution");
			added = true;
		}

		float offset = capsule.height * 0.5f;
		Float3 s1 = capsule.center - capsule.direction * offset;
		Float3 s2 = capsule.center + capsule.direction * offset;
		float r = sphere.radius + capsule.radius;

		Float3 cp;
		Float3 d = s2 - s1;
		Float3 v = sphere.center - s1;

		float s = Clamp( DotProduct( v, d ) / DotProduct( d, d ), 0.0f, 1.0f );
		
		cp = s1 + d * s;

		Float3 n = sphere.center - cp;
		float sqD = n.squaredMagnitude();
		
		if( sqD < r*r )
		{
			if( contact )
			{
				float d = sqrtf( sqD );
				float pd = r - d;

				if( d == 0.0f )
					return false;

				contact->Normal = n * (1.0f / d);
				contact->PenetrationDepth = pd;
				contact->Point[0] = sphere.center - contact->Normal * sphere.radius;
				contact->numPoints = 1;
			}

			return true;
		}

		return false;
	}

	bool SphereToAabbSolution( const Sphere& sphere, const Aabb& aabb, Contact* contact )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Sphere Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SphereToAabbSolution");
			added = true;
		}

		Float3 cp = sphere.center;

		for(int i = 0; i < 3; ++i)
		{
			if( cp.v[i] < aabb.center.v[i] - aabb.extents.v[i] )
				cp.v[i] = aabb.center.v[i] - aabb.extents.v[i];
			if( cp.v[i] > aabb.center.v[i] + aabb.extents.v[i] )
				cp.v[i] = aabb.center.v[i] + aabb.extents.v[i];
		}

		Float3 v = sphere.center - cp;
		float sqD = v.squaredMagnitude();
		
		if( sqD < sphere.radius * sphere.radius )
		{
			if( contact )
			{
				if( sqD > 0.0f )
				{
					float d = sqrtf(sqD);
					float pd = sphere.radius - d;

					contact->Normal = v * (1.0f / d);
					contact->PenetrationDepth = pd;
					contact->Point[0] = sphere.center - contact->Normal * sphere.radius;
					contact->numPoints = 1;
				}
				else
				{
					Float3 axis(0.0f, 0.0f, 0.0f);
					int bestAxis = 0;
					v = sphere.center - aabb.center;
					float bestDistance = v.x / aabb.extents.x;

					sqD = v.squaredMagnitude();

					if( sqD == 0.0f )
						return false;
				
					float newDistance = v.y / aabb.extents.y;
					if( abs(newDistance) > abs(bestDistance) )
					{
						bestDistance = newDistance;
						bestAxis = 1;
					}
					newDistance = v.z / aabb.extents.z;
					if( abs(newDistance) > abs(bestDistance) )
					{
						bestDistance = newDistance;
						bestAxis = 2;
					}

					axis.v[bestAxis] = (bestDistance < 0.0f ? -1.0f : 1.0f);
					float d = abs(v.v[bestAxis]);
					float r = sphere.radius + aabb.extents.v[bestAxis];
					float pd = r - d;

					contact->Normal = axis;
					contact->PenetrationDepth = pd;
					contact->Point[0] = sphere.center - contact->Normal * sphere.radius;
					contact->numPoints = 1;
				}
			}
			
			return true;
		}

		return false;
	}

	bool SphereToTriangleSolution( const Sphere& sphere, const Float3& a, const Float3& b, const Float3& c, Contact* contact )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Sphere Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SphereToTriangleSolution");
			added = true;
		}

		Float3 cp;
		float sqD = ClosestPtTriangleSolution( sphere.center, a, b, c, cp );
		
		if( sqD < sphere.radius * sphere.radius )
		{
			if( contact )
			{
				float d = sqrtf(sqD);
				float pd = sphere.radius - d;

				if( d == 0.0f || pd < 0.001f )
					return false;

				Float3 tN;
				CrossProduct( tN, b-a, c-b );
				Float3 N = (sphere.center - cp) * (1.0f / d);;

				if( DotProduct( tN, N ) < 0.0f )
					return false;

				contact->Normal = N;
				contact->PenetrationDepth = pd;
				contact->Point[0] = sphere.center - contact->Normal * sphere.radius;
				contact->numPoints = 1;
			}

			return true;
		}

		return false;
	}

	bool SphereToObbSolution( const Sphere& sphere, const Obb& obb, Contact* contact  )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Sphere Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SphereToObbSolution");
			added = true;
		}

		Aabb aabb;
		aabb.center.makeZero();
		aabb.extents = obb.extents;

		Sphere oSphere = sphere;
		InvTransformPoint( oSphere.center, sphere.center, obb.transform );

		if( SphereToAabbSolution( oSphere, aabb, contact ) )
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

	/***/
}