#pragma once

#include "EDPhysics.h"
#include "Particle.h"
#include "../EDMath/float4x4.h"
#include "../EDCollision/EDCollision.h"

using namespace EDMath;

namespace EDPhysics
{
	class RigidBodyPhysics
	{
	public:

		// Structure to represent various data about a contact point
		struct ContactPointData
		{
			Float3 position;
			Float3 projection;
			float barycentric[4];
			float clampedBarycentric[4];
			float weights[4];
			Float3 normalToCenterOfMass;
			float invDistanceToCenterOfMass;
			Float3 previousPos;
			Float3 velocity;
			Float3 previousNormalToCenterOfMass;

			ContactPointData(void) : velocity(0.0f,0.0f,0.0f) {}
		};

		RigidBodyPhysics(void);

		void Update(void);

		// Set the transform for the rigid body
		void SetTransform(const Float4x4& transform);
		// Get the current transform for the rigid body.
		const Float4x4& GetTransform(void)const { return transform; }
		void GetObjectTransform(Float4x4& xform)const;
		
		static void resolveCollision(RigidBodyPhysics &lhs, RigidBodyPhysics& rhs, const EDCollision::Contact& state, MaterialId lhsMaterialId, MaterialId rhsMaterialId);
		static void resolveCollision(RigidBodyPhysics &lhs, const EDCollision::Contact& state, MaterialId lhsMaterialId, MaterialId rhsMaterialId);
		static void resolveCollision(RigidBodyPhysics* firstBody, RigidBodyPhysics* secondBody, const EDCollision::Contact& state, MaterialId firstMaterialId, MaterialId secondMaterialId);


		void ApplyForce(const Float3& f);
		void ApplyForceAtPoint(const Float3& f, const Float3& p);
		void ApplyDisplacementAtPoint(const Float3& d, const Float3& p);
		
		void CenterAt(const Float4x4& m);
		
		void SetCenterOfMass(const Float3& c){ localCoM = c; }
		const Float3& GetCenterOfMass(void){ return localCoM; }
		void GetVelocityAtPoint(const Float3& p, Float3& v);

		void MoveTo(const Float4x4& m);
		
		void Sleep(void);
		void Wake(void);
		bool IsAwake(void){ return !sleepState; }

		bool GetGravityState(void){ return gravity; }
		void SetGravityState(bool g){ gravity = g; }

		float GetInverseMass(void){ return inverseMass; }
		void SetInverseMass(float i){ inverseMass = i; }

		// Applies a collision reaction to a rigid body,
		// given the displacement (correction delta),
		// the opposing force, and the ContactPointData for the collision.
		static void applyReaction(RigidBodyPhysics& rigidBody, const Float3& displacement, const Float3& opposingForce, const ContactPointData& cpData);

		// Calculates various information about a contact point on the bounds of the rigid body.
		void getContactPointData(ContactPointData& data, const Float3& point);
		void getContactPointData(ContactPointData& data, const Float3* points, unsigned int numPoints);

		const Float3& GetLinearFactors(void)const{ return linearFactors; }
		float GetAngularFactor(void)const{ return angularFactor; }

		void SetLinearFactors(const Float3& l)
		{
			linearFactors.x = max( 0.0f, min( 1.0f, l.x ) );
			linearFactors.y = max( 0.0f, min( 1.0f, l.y ) );
			linearFactors.z = max( 0.0f, min( 1.0f, l.z ) );
		}

		void SetAngularFactor(float a)
		{
			angularFactor = max( 0.0f, min( 1.0f, a ) );
		}

	private:

		// Object space points of the regular tetrahedron.
		// Also serve as the object space normals 
		// of the tetrahedron faces (pointing inward)
		static const Float3 localPoints[4];

		// Length of the stick constraints between each
		// pair of particles on the tetrahedron
		static const float restLength;
		
		// Inverse of the rest length 
		static const float inverseRestLength;
		// Rest length squared
		static const float restLengthSq;
		// sin( pi/3 )
		static const float sineOfPiOver3;
		// 1 / sin( pi/3 )
		static const float inverseSineOfPiOver3;

		static const float sleepEpsilon;
		static const float motionMaximum; 
		static const float motionMinimum;

		// Transformation matrix of the rigid body,
		// derived from the 4 points of the tetrahedron
		Float4x4 transform;

		// Weighted average representing the smallest
		// movement of the particles over time.
		// When motion drops below a threshold,
		// the rigid body is put to sleep.
		float motion;

		//Particle particles[4];
		Float3 particlePos[4];
		Float3 particlePrev[4];

		Float3 forces[4];

		Float3 localCoM;

		//Float3 linearConstraints;
		//Float3 rotationalConstraints;
		Float3 linearFactors;
		float angularFactor;

		bool sleepState;
		bool gravity;
		float inverseMass;

		// Calculate the transform from the particles
		// of the tetrahedron
		void buildTransform(void);

		static void LerpPos(Float3& out, const Float3* points, const float* barys);

		static void GetWeights(const float* barys, float* weights);

		// Get the barycentric coordinates for some point.
		void getBarycentricCoords(float* out, const Float3& point);

		// Calculate the clamped version of a set of barycentric coordinates.
		// The clamped coordinates represent the matching point in the
		// surface of the tetrahedron.
		static void clampBarycentricCoords(float* out, const float* barys);

		// Apply the stick constraints to all positions of all particle pairs.
		void applyConstraints(void);

		static void constrainPositions(Float3* pos);
	};
}