#include "precompiled.h"
#include "../EDUtilities/PrintConsole.h"

#include "../EDCollision/segment.h"
#include "../EDCollision/EDCollision.h"
#include "../EDCollision/ray.h"

using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDCollision
{
	float SquaredDistanceSolution( const Float3& p, const Float3& s1, const Float3& s2 )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SquaredDistanceSolution");
			added = true;
		}

		Float3 d = s2 - s1;
		Float3 v = p - s1;
		Float3 c;

		float s = Clamp( DotProduct( v, d ) / DotProduct( d, d ), 0.0f, 1.0f );
		
		c = s1 + d * s;

		return (p - c).squaredMagnitude();	
	}

	float ClosestPtSegmentSegmentSolution( const Segment& s1, const Segment& s2, Float3 &c1, Float3 &c2 )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtSegmentSegmentSolution");
			added = true;
		}

		return ClosestPtSegmentSegmentSolution( s1.points[0], s1.points[1], s2.points[0], s2.points[1], c1, c2 );
	}

	float ClosestPtSegmentSegmentSolution( const Float3 &p1, const Float3 &q1, const Float3 &p2, const Float3 &q2, Float3 &c1, Float3 &c2 )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtSegmentSegmentSolution");
			added = true;
		}

		float s, t;
		Float3 d1 = q1 - p1;
		Float3 d2 = q2 - p2;
		Float3 r = p1 - p2;
		float a = DotProduct( d1, d1 );
		float e = DotProduct( d2, d2 );
		float f = DotProduct( d2, r );

		if( a <= ED_EPSILON && e <= ED_EPSILON )
		{
			s = t = 0.0f;
			c1 = p1; 
			c2 = p2;
			return DotProduct( c1 - c2, c1 - c2 );
		}
		if( a <= ED_EPSILON )
		{
			s = 0.0f;
			t = f / e;
			t = Clamp( t, 0.0f, 1.0f );
		}
		else
		{
			float c = DotProduct( d1, r );
			if( e <= ED_EPSILON )
			{
				t = 0.0f;
				s = Clamp( -c / a, 0.0f, 1.0f );
			}
			else
			{
				float b = DotProduct( d1, d2 );
				float denom = a*e - b*b;
			
				if( denom != 0.0f )
				{
					s = Clamp( (b*f - c*e) / denom, 0.0f, 1.0f );
				}
				else
					s = 0.0f;

				t = (b*s + f) / e;

				if( t < 0.0f )
				{
					t = 0.0f;
					s = Clamp( -c / a, 0.0f, 1.0f );
				}
				else if( t > 1.0f )
				{
					t = 1.0f;
					s = Clamp( (b - c)/a, 0.0f, 1.0f );
				}
			}
		}

		c1 = p1 + d1 * s;
		c2 = p2 + d2 * t;
		return DotProduct( c1 - c2, c1 - c2 );
	}

	float ClosestPtSegmentSolution( const Float3 &p, const Segment& s, Float3& c )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtSegmentSolution");
			added = true;
		}

		return ClosestPtSegmentSolution( p, s.points[0], s.points[1], c );
	}

	float ClosestPtSegmentSolution( const Float3 &p, const Float3& s1, const Float3& s2, Float3& c )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtSegmentSolution");
			added = true;
		}

		Float3 d = s2 - s1;
		Float3 v = p - s1;

		float s = Clamp( DotProduct( v, d ) / DotProduct( d, d ), 0.0f, 1.0f );
		
		c = s1 + d * s;

		return (p - c).squaredMagnitude();
	}

	float ClosestPtSegmentTriangleSolution( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c, Float3& cp1, Float3& cp2 )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtSegmentTriangleSolution");
			added = true;
		}

		float sqDistance = FLT_MAX;

		//if( SegmentToTriangle( p, q, a, b, c, cp1 ) )
		//{
		//	cp2 = cp1;
		//	return 0.0f;
		//}

		Float3 barys;
		float t;
		
		Float3 dir = q - p;
		float maxLen = dir.magnitude();
		dir *= (1.0f / maxLen );
		if( IntersectRayTriangle( p, dir, maxLen, a, b, c, barys, t ) )
		{
			cp2 = cp1 = (p + dir * t);
			return 0.0f;
		}

		Float3 N;
		CrossProduct( N, b-a, c-b );

		Float3 projP = p - N * (DotProduct( N, p - a) / DotProduct( N, N ));

		Float3 edgeN;
		CrossProduct( edgeN, b-a, N );

		if( DotProduct( projP - a, edgeN ) < 0.0f )
		{
			CrossProduct( edgeN, c-b, N );
			if( DotProduct( projP - b, edgeN ) < 0.0f )
			{
				CrossProduct( edgeN, a-c, N );
				if( DotProduct( projP - c, edgeN ) < 0.0f )
				{
					cp1 = p;
					cp2 = projP;
					sqDistance = (cp1 - cp2).squaredMagnitude();
				}
			}
		}

		Float3 projQ = q - N * (DotProduct( N, q - a) / DotProduct( N, N ));

		CrossProduct( edgeN, b-a, N );

		float dSq = (q - projQ).squaredMagnitude();

		if( dSq < sqDistance )
		{
			if( DotProduct( projQ - a, edgeN ) < 0.0f )
			{
				CrossProduct( edgeN, c-b, N );
				if( DotProduct( projQ - b, edgeN ) < 0.0f )
				{
					CrossProduct( edgeN, a-c, N );
					if( DotProduct( projQ - c, edgeN ) < 0.0f )
					{
						cp1 = q;
						cp2 = projQ;
						sqDistance = dSq;
					}
				}
			}
		}

		Float3 cpS;
		Float3 cpE;
	
		dSq = ClosestPtSegmentSegmentSolution( p, q, a, b, cpS, cpE );
		if( dSq < sqDistance )
		{
			sqDistance = dSq;
			cp1 = cpS;
			cp2 = cpE;
		}

		dSq = ClosestPtSegmentSegmentSolution( p, q, b, c, cpS, cpE );
		if( dSq < sqDistance )
		{
			sqDistance = dSq;
			cp1 = cpS;
			cp2 = cpE;
		}

		dSq = ClosestPtSegmentSegmentSolution( p, q, c, a, cpS, cpE );
		if( dSq < sqDistance )
		{
			sqDistance = dSq;
			cp1 = cpS;
			cp2 = cpE;
		}

		return sqDistance;
	}

	float ClosestPtSegmentAabbSolution( const Float3& p, const Float3& q, const Float3& e, Float3& c1, Float3& c2 )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtSegmentAabbSolution");
			added = true;
		}

		float sqDist = FLT_MAX;

		int testAxes[3][2] = { {1,2}, {0,2}, {0,1} };

		for(int i = 0; i < 3; ++i)
		{
			int axis1 = testAxes[i][0];
			int axis2 = testAxes[i][1];

			if( p.v[axis1] < e.v[axis1] && p.v[axis1] > -e.v[axis1] && p.v[axis2] < e.v[axis2] && p.v[axis2] > -e.v[axis2] )
			{
				float dmin = p.v[i] + e.v[i];
				dmin *= dmin;

				if(dmin < sqDist)
				{
					sqDist = dmin;
					c1 = p;
					c2.v[i] = -e.v[i];
					c2.v[axis1] = p.v[axis1];
					c2.v[axis2] = p.v[axis2];
				}

				float dmax = p.v[i] - e.v[i];
				dmax *= dmax;

				if(dmax < sqDist)
				{
					sqDist = dmax;
					c1 = p;
					c2.v[i] = e.v[i];
					c2.v[axis1] = p.v[axis1];
					c2.v[axis2] = p.v[axis2];
				}
			}

			if( q.v[axis1] < e.v[axis1] && q.v[axis1] > -e.v[axis1] && q.v[axis2] < e.v[axis2] && q.v[axis2] > -e.v[axis2] )
			{
				float dmin = q.v[i] + e.v[i];
				dmin *= dmin;

				if(dmin < sqDist)
				{
					sqDist = dmin;
					c1 = q;
					c2.v[i] = -e.v[i];
					c2.v[axis1] = q.v[axis1];
					c2.v[axis2] = q.v[axis2];
				}

				float dmax = q.v[i] - e.v[i];
				dmax *= dmax;

				if(dmax < sqDist)
				{
					sqDist = dmax;
					c1 = q;
					c2.v[i] = e.v[i];
					c2.v[axis1] = q.v[axis1];
					c2.v[axis2] = q.v[axis2];
				}
			}
		}

		float scale[2] = { -1.0f, 1.0f };
		for(int i = 0; i < 3; ++i)
		{
			int axis1 = testAxes[i][0];
			int axis2 = testAxes[i][1];

			for(int j = 0; j < 2; ++j)
				for(int k = 0; k < 2; ++k)
				{
					Float3 p2;
					Float3 q2;

					p2.v[i] = -e.v[i];
					p2.v[axis1] = e.v[axis1] * scale[j];
					p2.v[axis2] = e.v[axis2] * scale[k];

					q2.v[i] = e.v[i];
					q2.v[axis1] = e.v[axis1] * scale[j];
					q2.v[axis2] = e.v[axis2] * scale[k];

					Float3 r1;
					Float3 r2;
					float result = ClosestPtSegmentSegment(p, q, p2, q2, r1, r2 );

					if( result < sqDist )
					{
						sqDist = result;
						c1 = r1;
						c2 = r2;
					}
				}
		}

		return sqDist;
	}

	float ClosestPtSegmentPlaneSolution( const Float3& p, const Float3& q, const Float3& n, float offset, Float3& cp1, Float3& cp2  )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtSegmentPlaneSolution");
			added = true;
		}

		Float3 d = q - p;
		float ratio = (offset - DotProduct( p, n )) / DotProduct( d, n );
		
		ratio = Clamp( ratio, 0.0f, 1.0f );

		cp1 = p + d * ratio;

		float signedDistance = DotProduct( n, cp1 ) - offset;

		cp2 = cp1 - n * signedDistance;
		
		return signedDistance*signedDistance;
	}

	float ClosestPtSegmentPlaneSolution( const Float3& p, const Float3& q, const Float3& n, const Float3& a, Float3& cp1, Float3& cp2 )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtSegmentPlaneSolution");
			added = true;
		}

		Float3 d = q - p;
		Float3 v = a - p;

		float ratio = DotProduct(v, n) / DotProduct(d, n);

		ratio = Clamp( ratio, 0.0f, 1.0f );

		cp1 = p + d * ratio;

		cp2 = cp1 - n * (DotProduct( cp1 - a, n) / DotProduct(n, n));
		
		return (cp1 - cp2).squaredMagnitude();
	}

	bool SegmentToSphereSolution( const Float3& p, const Float3& q, const Sphere& sphere )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SegmentToSphereSolution");
			added = true;
		}

		Float3 c;
		return ( ClosestPtSegment( sphere.center, p, q, c ) < sphere.radius * sphere.radius );
	}

	bool SegmentToSphereSolution( const Float3& p, const Float3& q, const Sphere& sphere, Contact& contact )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SegmentToSphereSolution");
			added = true;
		}

		Float3 c;

		float dSq = ClosestPtSegment( sphere.center, p, q, c );
		
		if( dSq > sphere.radius * sphere.radius )
			return false;

		float d = sqrtf(dSq);
		float pd = sphere.radius - d;
		
		if( d == 0 || pd < 0.001f )
			return false;

		contact.Normal = (c - sphere.center) * (1.0f / d);
		contact.Point[0] = c;
		contact.PenetrationDepth = pd;

		return true;
	}

	bool SegmentToCapsuleSolution( const Float3& p, const Float3& q, const Capsule& capsule )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SegmentToCapsuleSolution");
			added = true;
		}

		float offset = capsule.height * 0.5f;
		Float3 s1 = capsule.center - capsule.direction * offset;
		Float3 s2 = capsule.center + capsule.direction * offset;

		Float3 c1;
		Float3 c2;

		return ( ClosestPtSegmentSegmentSolution( p, q, s1, s2, c1, c2 ) < capsule.radius * capsule.radius );
	}

	bool SegmentToCapsuleSolution( const Float3& p, const Float3& q, const Capsule& capsule, Contact& contact )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SegmentToCapsuleSolution");
			added = true;
		}

		float offset = capsule.height * 0.5f;
		Float3 s1 = capsule.center - capsule.direction * offset;
		Float3 s2 = capsule.center + capsule.direction * offset;

		Float3 c1;
		Float3 c2;

		float dSq = ClosestPtSegmentSegment( p, q, s1, s2, c1, c2 );
		
		if( dSq > capsule.radius * capsule.radius )
			return false;

		float d = sqrtf(dSq);
		float pd = capsule.radius - d;

		if( d == 0 || pd < 0.001f )
			return false;

		contact.Normal = (c1 - c2) * (1.0f / d);
		contact.Point[0] = c1;
		contact.PenetrationDepth = pd;
		contact.numPoints = 1;

		return true;
	}

	bool SegmentToAabbSolution( const Float3& p, const Float3& q, const Aabb& aabb )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SegmentToAabbSolution");
			added = true;
		}

		Float3 m = (p + q) * 0.5f;
		Float3 d = q - m;
		m = m - aabb.center;

		float adx = abs(d.x);
		if( abs(m.x) > aabb.extents.x + adx ) 
			return false;

		float ady = abs(d.y);
		if( abs(m.y) > aabb.extents.y + ady ) 
			return false;

		float adz = abs(d.z);
		if( abs(m.z) > aabb.extents.z + adz ) 
			return false;

		adx += ED_EPSILON;
		ady += ED_EPSILON;
		adz += ED_EPSILON;

		if( abs(m.y * d.z - m.z * d.y) > aabb.extents.y * adz + aabb.extents.z * ady )
			return false;

		if( abs(m.z * d.x - m.x * d.z) > aabb.extents.x * adz + aabb.extents.z * adx )
			return false;

		if( abs(m.x * d.y - m.y * d.x) > aabb.extents.x * ady + aabb.extents.y * adx )
			return false;

		return true;
	}

	bool SegmentToObbSolution( const Float3& p, const Float3& q, const Obb& obb )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SegmentToObbSolution");
			added = true;
		}

		Float3 po;
		InvTransformPoint( po, p, obb.transform ); 
		Float3 qo;
		InvTransformPoint( qo, q, obb.transform );

		Float3 m = (po + qo) * 0.5f;
		Float3 d = qo - m;

		float adx = abs(d.x);
		if( abs(m.x) > obb.extents.x + adx ) 
			return false;

		float ady = abs(d.y);
		if( abs(m.y) > obb.extents.y + ady ) 
			return false;

		float adz = abs(d.z);
		if( abs(m.z) > obb.extents.z + adz ) 
			return false;

		adx += ED_EPSILON;
		ady += ED_EPSILON;
		adz += ED_EPSILON;

		if( abs(m.y * d.z - m.z * d.y) > obb.extents.y * adz + obb.extents.z * ady )
			return false;

		if( abs(m.z * d.x - m.x * d.z) > obb.extents.x * adz + obb.extents.z * adx )
			return false;

		if( abs(m.x * d.y - m.y * d.x) > obb.extents.x * ady + obb.extents.y * adx )
			return false;

		return true;
	}

	bool SegmentToTriangleSolution( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SegmentToTriangleSolution");
			added = true;
		}

		Float3 p0;
		Float3 p1;

		for( int i = 0; i < 3; ++i)
		{
			if( c.v[i] < b.v[i] )
			{
				p0 = c;
				p1 = b;
				break;
			}
			else if( b.v[i] < c.v[i] )
			{
				p0 = b;
				p1 = c;
				break;
			}
		}

		Float3 s0 = p0 - p;
		Float3 s1 = p1 - p;
		Float3 N0;
		CrossProduct( N0, s0, s1 );
		
		for( int i = 0; i < 3; ++i)
		{
			if( a.v[i] < c.v[i] )
			{
				p0 = a;
				p1 = c;
				break;
			}
			else if( c.v[i] < a.v[i] )
			{
				p0 = c;
				p1 = a;
				break;
			}
		}

		s0 = p0 - p;
		s1 = p1 - p;
		Float3 N1;
		CrossProduct( N1, s0, s1 );

		for( int i = 0; i < 3; ++i)
		{
			if( b.v[i] < a.v[i] )
			{
				p0 = b;
				p1 = a;
				break;
			}
			else if( a.v[i] < b.v[i] )
			{
				p0 = a;
				p1 = b;
				break;
			}
		}

		s0 = p0 - p;
		s1 = p1 - p;
		Float3 N2;
		CrossProduct( N2, s0, s1 );

		Float3 d = q - p;

		float result0 = DotProduct( d, N0 );
		float result1 = DotProduct( d, N1 );
		float result2 = DotProduct( d, N2 );

		if( ( result0 < 0.0f && result1 < 0.0f && result2 < 0.0f ) ||
			( result0 > 0.0f && result1 > 0.0f && result2 > 0.0f ) )
		{
			Float3 N;
			CrossProduct( N, b-a, c-b );
			
			float ratio = DotProduct( s0, N ) / DotProduct( d, N );

			return ( ratio > 0.0f && ratio < 1.0f );
		}

		return false;
	}

	bool SegmentToTriangleSolution( const Float3& p, const Float3& q, const Float3& a, const Float3& b, const Float3& c, Float3& cp )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SegmentToTriangleSolution");
			added = true;
		}

		Float3 p0;
		Float3 p1;

		for( int i = 0; i < 3; ++i)
		{
			if( c.v[i] < b.v[i] )
			{
				p0 = c;
				p1 = b;
				break;
			}
			else if( b.v[i] < c.v[i] )
			{
				p0 = b;
				p1 = c;
				break;
			}
		}

		Float3 s0 = p0 - p;
		Float3 s1 = p1 - p;
		Float3 N0;
		CrossProduct( N0, s0, s1 );
		
		for( int i = 0; i < 3; ++i)
		{
			if( a.v[i] < c.v[i] )
			{
				p0 = a;
				p1 = c;
				break;
			}
			else if( c.v[i] < a.v[i] )
			{
				p0 = c;
				p1 = a;
				break;
			}
		}

		s0 = p0 - p;
		s1 = p1 - p;
		Float3 N1;
		CrossProduct( N1, s0, s1 );

		for( int i = 0; i < 3; ++i)
		{
			if( b.v[i] < a.v[i] )
			{
				p0 = b;
				p1 = a;
				break;
			}
			else if( a.v[i] < b.v[i] )
			{
				p0 = a;
				p1 = b;
				break;
			}
		}

		s0 = p0 - p;
		s1 = p1 - p;
		Float3 N2;
		CrossProduct( N2, s0, s1 );

		Float3 d = q - p;

		float u = DotProduct( d, N0 );
		float v = DotProduct( d, N1 );
		float w = DotProduct( d, N2 );

		if( ( u <= 0.0f && v <= 0.0f && w <= 0.0f ) ||
			( u >= 0.0f && v >= 0.0f && w >= 0.0f ) )
		{
			float denom = 1.0f / (u + v + w );

			u *= denom;
			v *= denom;
			w *= denom;

			cp = a * u + b * v + c * w;

			float ratio = DotProduct( cp - p, d ) / DotProduct( d, d );

			return (ratio > 0.0f && ratio < 1.0f);
			//Float3 N;
			//CrossProduct( N, b-a, c-b );
			//
			//float ratio = DotProduct( s0, N ) / DotProduct( d, N );

			//if( ratio > 0.0f && ratio < 1.0f )
			//{
			//	cp = p + d * ratio;
			//	return true;
			//}
		}

		return false;
	}

	bool SegmentToPlaneSolution( const Float3& p, const Float3& q, const Float3& n, float offset )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SegmentToPlaneSolution");
			added = true;
		}

		Float3 d = q - p;
		float ratio = (offset - DotProduct( p, n )) / DotProduct( d, n );

		return (ratio > 0.0f && ratio < 1.0f);
	}

	bool SegmentToPlaneSolution( const Float3& p, const Float3& q, const Float3& n, float offset, Float3& cp )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Segment Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SegmentToPlaneSolution");
			added = true;
		}

		Float3 d = q - p;
		float ratio = (offset - DotProduct( p, n )) / DotProduct( d, n );
		
		if( ratio > 0.0f && ratio < 1.0f )
		{
			cp = p + d * ratio;
			return true;
		}

		return false;
	}
}