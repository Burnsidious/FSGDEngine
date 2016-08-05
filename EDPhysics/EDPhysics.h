#pragma once

#include "../EDMath/float3.h"
#include "../EDMath/float4x4.h"
using namespace EDMath;

namespace EDPhysics
{
	// Number of steps per 1/60th of a second
	const unsigned int STEP_COUNT = 2;

	// Fixed time step for verlet integration
	const float TIME_STEP = (1.0f / (STEP_COUNT * 60.0f));

	// Fixed time step squared
	const float TIME_STEP_SQ = (TIME_STEP*TIME_STEP);

	const float EARTH_GRAVITY = 20.0f;
	const float EARTH_GRAVITY_DELTA = EARTH_GRAVITY * TIME_STEP_SQ;

	// Material IDs, used to determine proper friction and restitution
	//enum MaterialId{ MATERIAL_DEFAULT = 0, MATERIAL_COUNT };
	typedef unsigned int MaterialId;
	static const MaterialId MATERIAL_DEFAULT = 0;

	struct PhysicsMaterial
	{
		float friction;
		float friction2;
		float bounciness;
		Float3 frictionDirection2;

		PhysicsMaterial(void);
	};

	const PhysicsMaterial& GetPhysicsMaterial(MaterialId id);
	void AddPhysicsMaterial(MaterialId id, const PhysicsMaterial& mat);

	void getOpposingForces(Float3& friction, Float3& restitution, const Float3& velocity, const Float3& contactNormal);
	void getOpposingForces(Float3& friction, Float3& restitution, const Float3& velocity, const Float3& contactNormal, float cof, float cor);

	void calculateReaction(Float3& opposingForce, const Float3& velocty, const Float3& contactNormal, MaterialId lhsMaterial, MaterialId rhsMaterial, const Float4x4& lhsTransform, const Float4x4& rhsTransform );

	// Apply a stick constraint on two points, forcing them to be a specified distance apart.
	// Takes in the points to modify, and the *SQUARED* distance they should be separated by.
	void applyStickConstraint(Float3& pointA, Float3& pointB, float restLengthSq);
	
	// Decompose a vector into two component vectors based on a normalized axis.
	// Provides the parallel component and the tangential (perpendicular) component.
	void decomposeVector(Float3& parallel, Float3& tangent, const Float3& vector, const Float3& axis);

	// Returns the inverse square root of x
	inline float InvSqrt(float x){ return 1.0f/sqrtf(x); }
}