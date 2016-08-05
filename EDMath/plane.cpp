#include "plane.h"

namespace EDMath
{
	void ComputePlane(Plane& p, const Float3& a, const Float3& b, const Float3& c)
	{
		CrossProduct( p.normal, b-a, c-b );
		p.normal.normalize();
		p.distance = DotProduct( p.normal, a );
	}

	Plane ComputePlane(float a, float b, float c, float d)
	{
		Plane result = { Float3(a,b,c), d };

		float mSq = DotProduct( result.normal, result.normal );

		//if( abs(1.0f - mSq) > 0.0001f )
		{
			float invMag = 1.0f / sqrt(mSq);
			result.normal *= invMag;
			result.distance *= invMag;
		}

		return result;
	}

	void TransformPlane(Plane& out, const Plane& in, const Float4x4& transform)
	{
		TransformVector( out.normal, in.normal, transform );
		out.distance = in.distance + DotProduct( out.normal, transform.WAxis );
	}
}