#include "precompiled.h"
#include "../EDUtilities/PrintConsole.h"

#include "../EDCollision/EDCollision.h"

using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDCollision
{
	float ClosestPtTriangleSolution( const Float3& p, const Float3& a, const Float3& b, const Float3& c, Float3& closest )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "Triangle Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ClosestPtTriangleSolution");
			added = true;
		}

		Float3 ab = b -a;
		Float3 ac = c - a;
		Float3 ap = p - a;
		float d1 = DotProduct(ab, ap);
		float d2 = DotProduct(ac, ap);
		
		if( d1 <= 0.0f && d2 <= 0.0f )
		{
			closest = a;
			return DotProduct(ap, ap);
		}

		Float3 bp = p - b;
		float d3 = DotProduct(ab, bp);
		float d4 = DotProduct(ac, bp);
		
		if( d3 >= 0.0f && d4 <= d3 )
		{
			closest = b;
			return DotProduct(bp, bp);
		}

		float vc = d1*d4 - d3*d2;
		if( vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f )
		{
			float v = d1 / (d1 - d3);
			closest = a + ab * v;
			return (closest - p).squaredMagnitude();
		}
		
		Float3 cp = p - c;
		float d5 = DotProduct(ab, cp);
		float d6 = DotProduct(ac, cp);
		if( d6 >= 0.0f && d5 <= d6)
		{
			closest = c;
			return DotProduct( cp, cp );
		}

		float vb = d5*d2 - d1*d6;
		if( vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f )
		{
			float w = d2 / (d2 - d6);
			closest = a + ac * w;
			return (closest - p).squaredMagnitude();
		}

		float va = d3*d6 - d5*d4;
		if( va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f )
		{
			float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
			closest = b + (c - b) * w;
			return (closest - p).squaredMagnitude();
		}

		float denom = 1.0f / (va + vb + vc);
		float v = vb * denom;
		float w = vc * denom;
		
		closest = a + ab * v + ac * w;
		return (closest - p).squaredMagnitude();
		
		//Float3 edges[3] = { b - a, c - b, a - c };
		//Float3 verts[3] = {a, b, c};

		//Float3 N;
		//CrossProduct( N, edges[0], edges[1] );

		//Float3 v = p - a;
		//
		//float ratio = DotProduct( v, N ) / DotProduct( N, N );

		//Float3 pt = p - N * ratio;

		//for( int i = 0; i < 3; ++i )
		//{
		//	Float3 edgeN;
		//	CrossProduct( edgeN, edges[i], N );
		//	
		//	v = pt - verts[i];

		//	if( DotProduct( v, edgeN ) < 0.0f )
		//		continue;

		//	float ratio = DotProduct( v, edges[i] ) / DotProduct( edges[i], edges[i] );
		//	ratio = Clamp( ratio, 0.0f, 1.0f );
		//	cp = verts[i] + edges[i] * ratio;
		//	return (cp - p).squaredMagnitude();
		//}

		//cp = pt;
		//return (cp - p).squaredMagnitude();
	}
}