#include <algorithm>
#include "RigidBody.h"

namespace EDPhysics
{
	const float RigidBodyPhysics::sleepEpsilon = EARTH_GRAVITY_DELTA * 0.00390625f;
	const float RigidBodyPhysics::motionMaximum = sleepEpsilon * 10.0f;
	const float RigidBodyPhysics::motionMinimum = sleepEpsilon * 2.0f;

	const Float3 RigidBodyPhysics::localPoints[4] =
	{
		Float3(0.0f, 1.0f, 0.0f),
		Float3((sqrt(2.0f) / 3.0f)*2.0f, -1.0f/3.0f, 0.0f),
		Float3(-(sqrt(2.0f) / 3.0f), -1.0f/3.0f, sqrt(2.0f/3.0f)),
		Float3(-(sqrt(2.0f) / 3.0f), -1.0f/3.0f, -sqrt(2.0f/3.0f))
	};

	const float RigidBodyPhysics::restLength = 2.0f * sqrt(2.0f/3.0f);
	const float RigidBodyPhysics::inverseRestLength = 1.0f / restLength;
	const float RigidBodyPhysics::restLengthSq = restLength*restLength;
	const float RigidBodyPhysics::sineOfPiOver3 = sin( 3.14159265f / 3.0f );
	const float RigidBodyPhysics::inverseSineOfPiOver3 = 1.0f / sineOfPiOver3;

	RigidBodyPhysics::RigidBodyPhysics(void)
	{
		transform.makeIdentity();
		motion = motionMinimum;
		sleepState = false;
		gravity = true;
		inverseMass = 0.5f;

		linearFactors = Float3(1.0f, 1.0f, 1.0f);
		angularFactor = 1.0f;

		localCoM.makeZero();

		for(int i = 0; i < 4; ++i)
		{
			particlePos[i] = particlePrev[i] = localPoints[i];
			forces[i].makeZero();
		}
	}

	void RigidBodyPhysics::Update(void)
	{
		// This function is completed for you.
		
		// If the body is asleep, do nothing.
		if( sleepState )
			return;

		// Update the particle system...
		Float3 accels[4];

		for(int i = 0; i < 4; ++i)
		{
			accels[i].makeZero();


			if( gravity )
				accels[i].y = -EARTH_GRAVITY;

			accels[i] += forces[i] * inverseMass;
			forces[i].makeZero();
		}
		
		EDPhysics::Update(particlePos, particlePrev, 4, accels);

		// Apply the stick constraints to the particles
		// to maintain the tetrahedron shape.
		for(size_t iterCount = 0; iterCount < 4; ++iterCount)
			applyConstraints();

		// Update the transform of the rigid body.
		buildTransform();

		//The remaining code determines the smallest amount
		//of movement for the particles and adds it to a 
		//weighted average (motion). If the motion drops
		//below a minimum, the body is put to sleep.

		float deltaMag = 0.0f;
		for(size_t i = 0; i < 4; ++i)
		{
			Float3 delta = particlePos[i] - particlePrev[i];
		
			float diffSq = DotProduct( delta, delta );

			if( diffSq > deltaMag )
				deltaMag = diffSq;
		}

		const float bias = 0.8f;
		motion = bias*motion + (1.0f - bias)*deltaMag;

		if( motion > motionMaximum)
			motion = motionMaximum;

		if( motion < sleepEpsilon )
		{
			Sleep();
		}
	}

	void RigidBodyPhysics::Sleep(void)
	{
		motion = motionMinimum;
		sleepState = true;

		for(size_t i = 0; i < 4; ++i)
		{
			particlePrev[i] = particlePos[i];
		}
	}

	void RigidBodyPhysics::Wake(void)
	{
		if( sleepState )
		{
			motion = motionMinimum;
			sleepState = false;
		}
	}

	void RigidBodyPhysics::ApplyForce(const Float3& f)
	{
		Wake();
		for(int i = 0; i < 4; ++i)
			forces[i] += f;
	}

	void RigidBodyPhysics::ApplyForceAtPoint(const Float3& f, const Float3& p)
	{
		Wake();

		ContactPointData prj;
		getContactPointData( prj, p );

		Float3 posCorrectionN;
		Float3 posCorrectionT;
		decomposeVector( posCorrectionN, posCorrectionT, f, prj.normalToCenterOfMass );

		posCorrectionT *= min(1.0f, prj.invDistanceToCenterOfMass );

		for(int i = 0; i < 4; ++i)
		{
			forces[i] += posCorrectionN + posCorrectionT * prj.weights[i];
		}
	}

	void RigidBodyPhysics::ApplyDisplacementAtPoint(const Float3& d, const Float3& p)
	{
		Wake(); 

		ContactPointData prj;
		getContactPointData( prj, p );

		Float3 posCorrectionN;
		Float3 posCorrectionT;
		decomposeVector( posCorrectionN, posCorrectionT, d, prj.normalToCenterOfMass );

		posCorrectionT *= min(1.0f, prj.invDistanceToCenterOfMass );

		Float3 velCorrectionN;
		Float3 velCorrectionT;
		decomposeVector( velCorrectionN, velCorrectionT, prj.velocity, prj.previousNormalToCenterOfMass );

		velCorrectionT *= min(1.0f, prj.invDistanceToCenterOfMass );
		
		for(int i = 0; i < 4; ++i)
		{
			particlePos[i] += posCorrectionN + posCorrectionT * prj.weights[i];
			particlePrev[i] += velCorrectionN + velCorrectionT * prj.weights[i];
		}

		for(size_t iterCount = 0; iterCount < 4; ++iterCount)
			applyConstraints();

		buildTransform();
	}

	void RigidBodyPhysics::CenterAt(const Float4x4& m)
	{
		Float3 offset;
		TransformVector( offset, localCoM, m );
		
		Float4x4 m2 = m;
		m2.WAxis += offset;

		for(int i = 0; i < 4; ++i)
		{
			Float3 delta = particlePos[i] - particlePrev[i];
			
			TransformPoint( particlePos[i], localPoints[i], m2 );
			particlePrev[i] = particlePos[i] - delta;
		}

		transform = m2;
	}

	void RigidBodyPhysics::MoveTo(const Float4x4& m)
	{
		Float3 offset;
		TransformVector( offset, localCoM, m );
		
		Float4x4 m2 = m;
		m2.WAxis += offset;

		for(int i = 0; i < 4; ++i)
		{
			particlePrev[i] = particlePos[i];
			TransformPoint( particlePos[i], localPoints[i], m2 );
		}

		transform = m2;
		sleepState = false;
	}

	void RigidBodyPhysics::GetObjectTransform(Float4x4& xform)const
	{
		Float3 offset;
		TransformVector( offset, localCoM, transform );
		
		xform = transform;
		xform.WAxis -= offset;
	}

	void RigidBodyPhysics::buildTransform(void)
	{
		transform.ZAxis = particlePos[2] - particlePos[3];
		transform.ZAxis *= inverseRestLength;
		transform.padZ = 0.0f;

		transform.XAxis = particlePos[1] - particlePos[3];
		transform.XAxis *= inverseRestLength;
		transform.padX = 0.0f;

		CrossProduct( transform.YAxis, transform.ZAxis, transform.XAxis );
		transform.YAxis *= inverseSineOfPiOver3;
		transform.padY = 0.0f;

		CrossProduct( transform.XAxis, transform.YAxis, transform.ZAxis );

		transform.WAxis = (particlePos[0] + particlePos[1] + particlePos[2] + particlePos[3]) * 0.25f;
		transform.padW = 1.0f;
	}

	void RigidBodyPhysics::SetTransform(const Float4x4& xform)
	{
		transform = xform;
		sleepState = false;

		Float3 temp;
		TransformPoint( temp, localPoints[0], transform );
		particlePos[0] = particlePrev[0] = temp;

		TransformPoint( temp, localPoints[1], transform );
		particlePos[1] = particlePrev[1] = temp;

		TransformPoint( temp, localPoints[2], transform );
		particlePos[2] = particlePrev[2] = temp;

		TransformPoint( temp, localPoints[3], transform );
		particlePos[3] = particlePrev[3] = temp;
	}

	void RigidBodyPhysics::getBarycentricCoords(float* out, const Float3& point)
	{
		Float3 t = point - transform.WAxis;

		out[0] = DotProduct( particlePos[0] - transform.WAxis, t ) * 0.75f + 0.25f;
		out[1] = DotProduct( particlePos[1] - transform.WAxis, t ) * 0.75f + 0.25f;
		out[2] = DotProduct( particlePos[2] - transform.WAxis, t ) * 0.75f + 0.25f;
		out[3] = DotProduct( particlePos[3] - transform.WAxis, t ) * 0.75f + 0.25f;
	}

	void RigidBodyPhysics::clampBarycentricCoords(float* out, const float* barys)
	{
		float smallest = min( barys[0], min( barys[1], min(barys[2], barys[3]) ) );

		out[0] = barys[0] - smallest;
		out[1] = barys[1] - smallest;
		out[2] = barys[2] - smallest;
		out[3] = barys[3] - smallest;

		float invSum = 1.0f / (out[0] + out[1] + out[2] + out[3]);

		out[0] *= invSum;
		out[1] *= invSum;
		out[2] *= invSum;
		out[3] *= invSum;	
	}

	void RigidBodyPhysics::LerpPos(Float3& pos, const Float3* points, const float* barys)
	{
		pos = points[0] * barys[0] +
				points[1] * barys[1] +
				points[2] * barys[2] +
				points[3] * barys[3];
	}

	void RigidBodyPhysics::GetWeights(const float* barys, float* weights)
	{
		float lambda = 1.0f / (barys[0]*barys[0] +
								barys[1]*barys[1] +
								barys[2]*barys[2] +
								barys[3]*barys[3] );

		weights[0] = barys[0] * lambda;
		weights[1] = barys[1] * lambda;
		weights[2] = barys[2] * lambda;
		weights[3] = barys[3] * lambda;
	}

	void RigidBodyPhysics::constrainPositions(Float3* pos)
	{
		applyStickConstraint( pos[0], pos[1], restLengthSq );
		applyStickConstraint( pos[0], pos[2], restLengthSq );
		applyStickConstraint( pos[0], pos[3], restLengthSq );
		
		applyStickConstraint( pos[1], pos[2], restLengthSq );
		applyStickConstraint( pos[1], pos[3], restLengthSq );

		applyStickConstraint( pos[2], pos[3], restLengthSq );
	}

	void RigidBodyPhysics::applyConstraints(void)
	{
		Float3 newCenter = (particlePos[0] + particlePos[1] + particlePos[2] + particlePos[3]) * 0.25f;

		for(int i = 0; i < 4; ++i)
		{
			Float3 N = particlePos[i] - newCenter;

			Float3 lastPos;
			TransformVector( lastPos, localPoints[i], transform );
			Float3 delta = N - lastPos;

			for( int j = 0; j < 3; ++j)
				particlePos[i].v[j] -= delta.v[j] * (1.0f - angularFactor);
		}

		for(int i = 0; i < 4; ++i)
		{
			Float3 delta = particlePos[i] - particlePrev[i];

			for(int j = 0; j < 3; ++j)
				particlePrev[i].v[j] += delta.v[j] * (1.0f - linearFactors.v[j]);
		}

		constrainPositions( particlePos );
		constrainPositions( particlePrev );
	}

	void RigidBodyPhysics::getContactPointData(ContactPointData& data, const Float3* points, unsigned int numPoints)
	{
		float bestVelSq = -FLT_MAX;
		for(unsigned int i = 0; i < numPoints; ++i)
		{
			float b[4];
			getBarycentricCoords( b, points[i] );

			Float3 prev;
			LerpPos(prev, particlePrev, b);

			Float3 delta = points[i] - prev;
			
			if( delta.squaredMagnitude() > bestVelSq )
			{
				bestVelSq = delta.squaredMagnitude();
				data.velocity = delta;
				data.position = points[i];
				data.previousPos = prev;
				data.barycentric[0] = b[0];	
				data.barycentric[1] = b[1];
				data.barycentric[2] = b[2];	
				data.barycentric[3] = b[3];
			}
		}

		clampBarycentricCoords(data.clampedBarycentric, data.barycentric);

		GetWeights( data.clampedBarycentric, data.weights );

		LerpPos( data.projection, particlePos, data.clampedBarycentric );

		Float3 CoM = (particlePos[0] +
						particlePos[1] +
						particlePos[2] +
						particlePos[3] ) * 0.25f;

		Float3 contactToCoM = CoM - data.position;

		data.invDistanceToCenterOfMass = InvSqrt( contactToCoM.squaredMagnitude() );

		data.normalToCenterOfMass = contactToCoM * data.invDistanceToCenterOfMass;
		
		Float3 previousCoM = (particlePrev[0] +
								particlePrev[1] +
								particlePrev[2] +
								particlePrev[3] ) * 0.25f;

		data.previousNormalToCenterOfMass = (previousCoM - data.previousPos) * data.invDistanceToCenterOfMass;
	}

	void RigidBodyPhysics::getContactPointData(ContactPointData& data, const Float3& contactPoint)
	{
		getContactPointData(data, &contactPoint, 1);
	}

	void RigidBodyPhysics::applyReaction(RigidBodyPhysics& rigidBody, const Float3& displacement, const Float3& opposingForce, const ContactPointData& cpData)
	{
		rigidBody.Wake();

		float distanceScalar = min(1.0f, cpData.invDistanceToCenterOfMass);

		Float3 constrainedDisplacement(0.0f, 0.0f, 0.0f);

		Float3 posCorrectionN;
		Float3 posCorrectionT;
		decomposeVector( posCorrectionN, posCorrectionT, displacement, cpData.normalToCenterOfMass );

		posCorrectionT *= distanceScalar;

		Float3 prevCorrection = opposingForce;

		Float3 prevCorrectionN;
		Float3 prevCorrectionT;
		decomposeVector( prevCorrectionN, prevCorrectionT, prevCorrection, cpData.previousNormalToCenterOfMass );

		prevCorrectionT *= distanceScalar;

		for(size_t i = 0; i < 4; ++i)
		{
			Float3 posDelta = posCorrectionN + posCorrectionT * cpData.weights[i];
			Float3 prevDelta = prevCorrectionN + prevCorrectionT * cpData.weights[i];

			rigidBody.particlePos[i] += posDelta;
			rigidBody.particlePrev[i] += posDelta - prevDelta;
		}

		for(size_t iterCount = 0; iterCount < 4; ++iterCount)
			rigidBody.applyConstraints();

		rigidBody.buildTransform();
	}

	void RigidBodyPhysics::GetVelocityAtPoint(const Float3& p, Float3& v)
	{
		float b[4];
		getBarycentricCoords( b, p );

		Float3 prev;
		LerpPos( prev, particlePrev, b );

		v = p - prev;
	}

	void RigidBodyPhysics::resolveCollision(RigidBodyPhysics &lhs, RigidBodyPhysics& rhs, const EDCollision::Contact& state, MaterialId lhsMaterialId, MaterialId rhsMaterialId)
	{
		EDCollision::Contact rhsState = state;
		rhsState.negate();

		ContactPointData lhsContactData;
		ContactPointData rhsContactData;

		lhs.getContactPointData( lhsContactData, state.Point, state.numPoints );
		rhs.getContactPointData( rhsContactData, rhsState.Point, rhsState.numPoints );

		Float3 displacement = state.Normal * state.PenetrationDepth;

		float invMassA = lhs.inverseMass;
		float invMassB = rhs.inverseMass;

		float invMassRatio = 1.0f / (invMassA + invMassB);
		float invMassRatioA = invMassA * invMassRatio;
		float invMassRatioB = invMassB * invMassRatio;

		Float3 relativeVelocity = lhsContactData.velocity  - rhsContactData.velocity;

		Float3 opposingForce(0.0f, 0.0f, 0.0f);

		calculateReaction( opposingForce, relativeVelocity, state.Normal, lhsMaterialId, rhsMaterialId, lhs.transform, rhs.transform );

		Float3 dispA = displacement * invMassRatioA;
		Float3 oppA = opposingForce * invMassRatioA;
		Float3 dispB = displacement * -invMassRatioB;
		Float3 oppB = opposingForce * -invMassRatioB;

		const float MIN_DISPLACEMENT = 0.00009f;
		const float MIN_OPPOSING = 0.001f;

		bool moveA = false;
		if( /*lhs.IsAwake() ||*/ dispA.squaredMagnitude() > MIN_DISPLACEMENT || oppA.squaredMagnitude() > MIN_OPPOSING || state.Normal.y > 0.707f )
			moveA = true;
			//applyReaction( lhs, dispA, oppA, lhsContactData );

		bool moveB = false;
		if( /*rhs.IsAwake() ||*/ dispB.squaredMagnitude() > MIN_DISPLACEMENT || oppB.squaredMagnitude() > MIN_OPPOSING || state.Normal.y < -0.707f )
			moveB = true;
			//applyReaction( rhs, dispB, oppB, rhsContactData ); 

		if( moveA )
		{
			if( moveB )
			{
				applyReaction( lhs, dispA, oppA, lhsContactData );
				applyReaction( rhs, dispB, oppB, rhsContactData );
			}
			else
			{
				applyReaction( lhs, displacement, opposingForce, lhsContactData );
			}
		}
		else if( moveB )
		{
			applyReaction( rhs, displacement * -1.0f, opposingForce * -1.0f, rhsContactData );
		}
	}

	void RigidBodyPhysics::resolveCollision(RigidBodyPhysics &lhs, const EDCollision::Contact& state, MaterialId lhsMaterialId, MaterialId rhsMaterialId)
	{
		ContactPointData contactData;

		lhs.getContactPointData( contactData, state.Point, state.numPoints );

		Float3 displacement = state.Normal * state.PenetrationDepth;

		Float3 opposingForce(0.0f, 0.0f, 0.0f);

		Float4x4 identity;
		identity.makeIdentity();

		calculateReaction( opposingForce, contactData.velocity, state.Normal, lhsMaterialId, rhsMaterialId, lhs.transform, identity );

		if( displacement.squaredMagnitude() > 0.0001f || opposingForce.squaredMagnitude() > 0.0001f || state.Normal.y > 0.707f )
			applyReaction( lhs, displacement, opposingForce, contactData );
	}

	void RigidBodyPhysics::resolveCollision(RigidBodyPhysics* firstBody, RigidBodyPhysics* secondBody, const EDCollision::Contact& state, MaterialId firstMaterialId, MaterialId secondMaterialId)
	{
		if( firstBody )
		{
			if( secondBody )
				resolveCollision( *firstBody, *secondBody, state, firstMaterialId, secondMaterialId );
			else
				resolveCollision( *firstBody, state, firstMaterialId, secondMaterialId );
		}
	}
}