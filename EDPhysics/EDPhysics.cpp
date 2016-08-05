#include "EDPhysics.h"
#include <xmmintrin.h>
#include <map>

namespace
{
	std::map< EDPhysics::MaterialId, EDPhysics::PhysicsMaterial > physicsMaterialMap;
}

namespace EDPhysics
{
	PhysicsMaterial::PhysicsMaterial(void) : frictionDirection2(0.0f, 0.0f, 0.0f)
	{
		friction = 0.75f;
		friction2 = 0.0f;
		bounciness = 0.125f;
	}

	const PhysicsMaterial& GetPhysicsMaterial(MaterialId id)
	{
		//if( physicsMaterialMap.empty() )
		//{
		//	physicsMaterialMap.insert( std::make_pair( 0, PhysicsMaterial() ) );
		//	PhysicsMaterial mat2;
		//	mat2.frictionDirection2.x = 1.0f;
		//	mat2.friction = 0.25f;
		//	physicsMaterialMap.insert( std::make_pair( 1, mat2 ) );
		//}

		auto iter = physicsMaterialMap.find(id);

		if( iter == physicsMaterialMap.end() )
			return physicsMaterialMap.begin()->second;

		return iter->second;
	}

	void AddPhysicsMaterial(MaterialId id, const PhysicsMaterial& mat)
	{
		auto iter = physicsMaterialMap.find(id);

		if( iter == physicsMaterialMap.end() )
			physicsMaterialMap.insert( std::make_pair( id, mat ) );
		else
			iter->second = mat;
	}

	void getOpposingForces(Float3& friction, Float3& restitution, const Float3& velocity, const Float3& contactNormal)
	{
		friction.makeZero();
		restitution.makeZero();

		float nV = DotProduct( velocity, contactNormal );

		if( nV >= 0.0f )
			return;

		restitution = Float3( contactNormal * -nV );
		
		float fMax = abs(nV);
		Float3 tangentialVelocity = velocity - contactNormal * nV;

		friction = Float3( -tangentialVelocity.x, -tangentialVelocity.y, -tangentialVelocity.z );

		float ffSq = DotProduct( friction, friction );

		if( ffSq > fMax * fMax )
		{
			friction *= InvSqrt(ffSq);
			friction *= fMax;
		}
	}

	void getOpposingForces(Float3& friction, Float3& restitution, const Float3& velocity, const Float3& contactNormal, float cof, float cor)
	{
		friction.makeZero();
		restitution.makeZero();

		float nV = DotProduct( velocity, contactNormal );

		//nV += EARTH_GRAVITY_DELTA;
		
		if( nV >= 0.0f )
			return;

		restitution = Float3( contactNormal * -nV ) * (1.0f + cor);

		float fMax = abs(nV) * cof;
		Float3 tangentialVelocity = velocity - contactNormal * nV;

		friction = Float3( -tangentialVelocity.x, -tangentialVelocity.y, -tangentialVelocity.z );

		float ffSq = DotProduct( friction, friction );

		if( ffSq > fMax * fMax )
		{
			friction *= InvSqrt(ffSq);
			friction *= fMax;
		}
	}

	void calculateReaction(Float3& opposingForce, const Float3& velocity, const Float3& contactNormal, MaterialId lhsMaterial, MaterialId rhsMaterial, const Float4x4& lhsTransform, const Float4x4& rhsTransform )
	{
		opposingForce.makeZero();
		Float3 friction(0.0f, 0.0f, 0.0f);
		Float3 restitution(0.0f, 0.0f, 0.0f);

		//getOpposingForces( friction, restitution, velocty, contactNormal );

		const PhysicsMaterial& mat0 = GetPhysicsMaterial(lhsMaterial);
		const PhysicsMaterial& mat1 = GetPhysicsMaterial(rhsMaterial);

		getOpposingForces( friction, restitution, velocity, contactNormal, (mat0.friction + mat1.friction) * 0.5f, (mat0.bounciness + mat1.bounciness) * 0.5f );

		//restitution *= 1.0f + (mat0.bounciness + mat1.bounciness) * 0.5f;
		
		//friction *= (mat0.friction + mat1.friction) * 0.5f;

		//float magSq = mat0.frictionDirection2.squaredMagnitude();
		//if( magSq > 0.0f )
		//{
		//	Float3 fdw;
		//	TransformVector( fdw, mat0.frictionDirection2, lhsTransform );
		//	float ratio = DotProduct( friction, fdw );
		//	
		//	Float3 friction2P = fdw * ratio;
		//	Float3 friction2T = friction - friction2P;

		//	friction2T *= mat0.friction;
		//	friction2P *= mat0.friction2;

		//	friction = friction2T + friction2P;
		//}
		//else
		//	friction *= mat0.friction;

		//magSq = mat1.frictionDirection2.squaredMagnitude();
		//if( magSq > 0.0f )
		//{
		//	Float3 fdw;
		//	TransformVector( fdw, mat1.frictionDirection2, rhsTransform );
		//	float ratio = DotProduct( friction, fdw );// / magSq;
		//	
		//	Float3 friction2P = fdw * ratio;
		//	Float3 friction2T = friction - friction2P;

		//	friction2T *= mat1.friction;
		//	friction2P *= mat1.friction2;

		//	friction = friction2T + friction2P;
		//}
		//else
		//	friction *= mat1.friction;

		opposingForce = friction + restitution;
	}

	void applyStickConstraint(Float3& pointA, Float3& pointB, float restLengthSq)
	{
		// TO DO: Apply the stick constraint
		//
		// - See "Mass-Aggregate Physics.pptx"
		//
		// NOTES:
		// - Implement the more efficient approach that does not require a square root.
		Float3 delta = pointB - pointA;

		delta *= restLengthSq / ( DotProduct( delta, delta ) + restLengthSq ) - 0.5f;

		pointA -= delta;
		pointB += delta;
	}

	void decomposeVector(Float3& parallel, Float3& tangent, const Float3& vector, const Float3& axis)
	{
		parallel = axis * DotProduct( vector, axis );
		tangent = vector - parallel;
	}

	//float InvSqrt(float x)
	//{
	//	// SSE (SIMD) operations to find the inverse square root of x
	//	// Should be *much* faster than 1.0f/sqrt(x)
	//	//return _mm_cvtss_f32( _mm_rsqrt_ss( _mm_load_ss(&x) ) );

	//	return 1.0f/sqrtf(x);
	//}
}