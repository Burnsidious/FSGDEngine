#pragma once

#include "float3.h"
#include "float4x4.h"

namespace EDMath
{
	class Quaternion
	{
	public:
		
		union
		{
			float v[4];

			struct
			{
				float x;
				float y;
				float z;
				float w;
			};

			struct
			{
				float i;
				float j;
				float k;
				float r;
			};
		};

		Quaternion(void){}

		Quaternion(float i, float j, float k, float r) : r(r), i(i), j(j), k(k) {}

		Quaternion( Float3 Axis, float angle )
		{
			FromAxisAngle( *this, Axis, angle );
		}

		Quaternion( const Float4x4& M )
		{
			FromRotation( *this, M );
		}

		inline Quaternion& operator*=(Quaternion M)
		{
			Quaternion Q = *this;

			r = Q.r * M.r - Q.i * M.i - Q.j * M.j - Q.k * M.k;
			i = Q.r * M.i + Q.i * M.r + Q.j * M.k - Q.k * M.j;
			j = Q.r * M.j + Q.j * M.r + Q.k * M.i - Q.i * M.k;
			k = Q.r * M.k + Q.k * M.r + Q.i * M.j - Q.j * M.i;

			return *this;
		}

		inline friend Quaternion operator*(Quaternion Q, Quaternion M)
		{
			Quaternion Result;

			Result.r = Q.r * M.r - Q.i * M.i - Q.j * M.j - Q.k * M.k;
			Result.i = Q.r * M.i + Q.i * M.r + Q.j * M.k - Q.k * M.j;
			Result.j = Q.r * M.j + Q.j * M.r + Q.k * M.i - Q.i * M.k;
			Result.k = Q.r * M.k + Q.k * M.r + Q.i * M.j - Q.j * M.i;

			return Result;
		}

		inline Quaternion& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			w *= scalar;

			return *this;
		}

		inline friend Quaternion operator*(Quaternion Q, float scalar)
		{
			return Quaternion( Q.x * scalar, Q.y * scalar, Q.z * scalar, Q.w * scalar );
		}

		inline friend Quaternion operator/(Quaternion Q, float scalar)
		{
			return Q * (1.0f / scalar);
		}

		inline Quaternion& operator/=(float scalar)
		{
			return *this *= (1.0f / scalar);
		}

		inline friend Quaternion operator+( Quaternion A, Quaternion B )
		{
			return Quaternion( A.x+B.x, A.y+B.y, A.z+B.z, A.w+B.w );
		}

		inline friend Quaternion operator+( Quaternion Q, Float3 V )
		{
			Quaternion M( V.x, V.y, V.z, 0.0f );
		
			M *= Q;
							
			Q.r += M.r * 0.5f;
			Q.i += M.i * 0.5f;
			Q.j += M.j * 0.5f;
			Q.k += M.k * 0.5f;

			return Q;
		}

		inline Quaternion& operator+=( Float3 V )
		{
			Quaternion M( V.x, V.y, V.z, 0.0f );
		
			M *= (*this);

			r += M.r * 0.5f;
			i += M.i * 0.5f;
			j += M.j * 0.5f;
			k += M.k * 0.5f;

			return *this;
		}

		inline Quaternion& operator+=(Quaternion M)
		{
			x += M.x;
			y += M.y;
			z += M.z;
			w += M.w;

			return *this;
		}

		static void FromRotation( Quaternion& Q, const Float4x4& M );
		static void ToRotation( Quaternion Q, Float4x4& M );

		static void FromAxisAngle( Quaternion& Q, Float3 axis, float angle );
		static void ToAxisAngle( Quaternion Q, Float3& axis, float& angle );

		static Quaternion Slerp( Quaternion A, Quaternion B, float t );
		static Quaternion Nlerp( Quaternion A, Quaternion B, float t );

		inline friend float DotProduct( Quaternion A, Quaternion B )
		{
			return A.x*B.x + A.y*B.y + A.z*B.z + A.w*B.w;
		}

		inline friend float squaredMagnitude( Quaternion Q )
		{
			return DotProduct( Q, Q );
		}

		inline friend float magnitude( Quaternion Q )
		{
			return sqrt( squaredMagnitude( Q ) );
		}

		inline Quaternion& normalize(void)
		{
			return *this *= (1.0f / magnitude(*this) );
		}
	};
}