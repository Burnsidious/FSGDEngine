#pragma once

#include "Component.h"
#include "GameObject.h"
#include "../EDCollision/EDCollision.h"
#include "../EDPhysics/RigidBody.h"

namespace EDGameCore
{
	class RigidBody : public ComponentT<RigidBody>
	{
		friend class GameObject;
		friend class Game;
		friend class ICollider;
	public:
		RigidBody(void);
		RigidBody(const RigidBody& rhs);

		~RigidBody();

		ComponentType GetComponentType(void)const{ return RIGID_BODY; }

		void Update(void);

		float GetInverseMass(void);
		void SetInverseMass(float m);

		void SetKinematic(bool k);
		void SetUseGravity(bool g);
		void Sleep(void);
		void Wake(void);

		bool UsesGravity(void);
		bool IsKinematic(void);
		bool IsSleeping(void);

		void AddForce(const Float3& f);
		void ApplyForceAtPoint(const Float3& f, const Float3& p);
		void ApplyDisplacementAtPoint(const Float3& delta, const Float3& p);

		void SetCenterOfMass(const Float3& c);
		const Float3& GetCenterOfMass(void);
		void GetObjectTransform(Float4x4& xform)const;

		void GetVelocityAtPoint(const Float3& p, Float3& v);

		static void ResolveTireCollision(RigidBody* rigidBody, EDCollision::Contact& contact, float suspensionDistance, float radius, float& motorTorque, const Float4x4& tireMatrix);
		static void ResolveCharacterCollision(RigidBody* rigidBody, EDCollision::Contact& contact, float standY, float stopY, EDPhysics::MaterialId lhsMaterialId, EDPhysics::MaterialId rhsMaterialId );
		static void ResolveCollision(RigidBody* lhs, RigidBody* rhs, EDCollision::Contact& state, EDPhysics::MaterialId lhsMaterialId, EDPhysics::MaterialId rhsMaterialId);
		static void ResolveCollision(RigidBody* lhs, EDCollision::Contact& state, EDPhysics::MaterialId lhsMaterialId, EDPhysics::MaterialId rhsMaterialId);

		const Float3& GetLinearFactors(void)const;
		float GetAngularFactor(void)const;
		void SetLinearFactors(const Float3& l);
		void SetAngularFactor(float a);
		void CenterAt(const Float4x4& xform);

		RigidBody* clone(void)const
		{
			return new RigidBody(*this);
		}

	private:
		EDPhysics::RigidBodyPhysics rigidBody;
		
		bool kinematic;

		void LoadState(TiXmlElement* xmlElement);

		uint8_t sleepDelayCounter;
	};
}