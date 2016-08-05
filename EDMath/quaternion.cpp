#include "quaternion.h"
#include <assert.h>
#include <cmath>

namespace EDMath
{
	void Quaternion::FromRotation( Quaternion& q, const Float4x4& a )
	{
		float trace = a.m[0][0] + a.m[1][1] + a.m[2][2] + 1.0f;

		if( trace > 0.001f )
		{
			float s = 0.5f / sqrt(trace);
			q.w = 0.25f / s;
			q.x = ( a.m[2][1] - a.m[1][2] ) * s;
			q.y = ( a.m[0][2] - a.m[2][0] ) * s;
			q.z = ( a.m[1][0] - a.m[0][1] ) * s;
		}
		else
		{
			if ( a.m[0][0] > a.m[1][1] && a.m[0][0] > a.m[2][2] )
			{
				float s = 2.0f * sqrt( 1.0f + a.m[0][0] - a.m[1][1] - a.m[2][2] );
				q.x = 0.25f * s;
				q.y = (a.m[0][1] + a.m[1][0] ) / s;
				q.z = (a.m[0][2] + a.m[2][0] ) / s;
				q.w = (a.m[1][2] - a.m[2][1] ) / s;

			}
			else if (a.m[1][1] > a.m[2][2])
			{
				float s = 2.0f * sqrt( 1.0f + a.m[1][1] - a.m[0][0] - a.m[2][2] );
				q.x = (a.m[0][1] + a.m[1][0] ) / s;
				q.y = 0.25f * s;
				q.z = (a.m[1][2] + a.m[2][1] ) / s;
				q.w = (a.m[0][2] - a.m[2][0] ) / s;
			}
			else
			{
				float s = 2.0f * sqrt( 1.0f + a.m[2][2] - a.m[0][0] - a.m[1][1] );
				q.x = (a.m[0][2] + a.m[2][0] ) / s;
				q.y = (a.m[1][2] + a.m[2][1] ) / s;
				q.z = 0.25f * s;
				q.w = (a.m[0][1] - a.m[1][0] ) / s;
			}
		}
	}

	Quaternion Quaternion::Slerp( Quaternion A, Quaternion B, float t )
	{
		assert( t >= 0.0f && t <= 1.0f );

		Quaternion C(B);

		float cosTheta = DotProduct( A, B );

		if( cosTheta < 0.0f )
		{
			cosTheta = -cosTheta;
			C.x = -B.x;
			C.y = -B.y;
			C.z = -B.z;
			C.w = -B.w;
		}

		if( cosTheta > 0.99f )
			return  A * (1.0f - t) + B * t;
		
		float theta = std::acos(cosTheta);
		float sinTheta = std::sin(theta);

		return (A * std::sin( (1.0f - t) * theta ) + C * std::sin(t * theta)) / sinTheta;
	}

	Quaternion Quaternion::Nlerp( Quaternion A, Quaternion B, float t )
	{
		return ( A * (1.0f - t) + B * t ).normalize();
	}

	void Quaternion::ToRotation( Quaternion Q, Float4x4& M )
	{
		M.Xx = 1.0f - (2*Q.j*Q.j + 2*Q.k*Q.k);
		M.Yx = 2*Q.i*Q.j + 2*Q.k*Q.r;
		M.Zx = 2*Q.i*Q.k - 2*Q.j*Q.r;

		M.Xy = 2*Q.i*Q.j - 2*Q.k*Q.r;
		M.Yy = 1.0f - (2*Q.i*Q.i + 2*Q.k*Q.k);
		M.Zy = 2*Q.j*Q.k + 2*Q.i*Q.r;

		M.Xz = 2*Q.i*Q.k + 2*Q.j*Q.r;
		M.Yz = 2*Q.j*Q.k - 2*Q.i*Q.r;
		M.Zz = 1.0f - (2*Q.i*Q.i + 2*Q.j*Q.j);
	}

	void Quaternion::FromAxisAngle( Quaternion& Q, Float3 axis, float angle )
	{
		float halfAngle = angle / 2;
		float sinHalfAngle = sin(angle/2);

		Q.r = cos(angle/2);
		Q.i = axis.x * sinHalfAngle;
		Q.j = axis.y * sinHalfAngle;
		Q.k = axis.z * sinHalfAngle;
	}

	void Quaternion::ToAxisAngle( Quaternion Q, Float3& axis, float& angle )
	{
		if( Q.w > 1.0f )
			Q.normalize();

		angle = 2.0f * acos(Q.w);

		float s = sqrt( 1.0f - Q.w*Q.w );

		if( s < 0.001f )
		{
			axis.x = 1.0f;
			axis.y = 0.0f;
			axis.z = 0.0f;
		}
		else
		{
			s = 1.0f / s;
			axis.x = Q.x * s;
			axis.y = Q.y * s;
			axis.z = Q.z * s;
		}
	}
}