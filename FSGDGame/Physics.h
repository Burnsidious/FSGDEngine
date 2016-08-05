#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDCollision/CollidableTypes.h"

namespace Behaviors
{
	class Physics : public EDGOCA::BehaviorT<Physics>
	{
		struct EdgeConstraint
		{
			unsigned int indices[2];
			float restLength;
		};

	public:
		enum PhysicsFlags{ GRAVITY = 1, DOF_3 = 2, DOF_6 = 4 };

		Physics(void) : physicsFlags(GRAVITY|DOF_3) {}

		static void PopulateMessageMap(void);

		static void OnUpdate(EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message);
		static void OnApplyForce(EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message);
		static void OnApplyReaction(EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message);
		
		const EDGOCA::AttributeNameKey* GetRequiredAttributes(void);

		void OnAdd(void);

	private:
		
		unsigned int physicsFlags;
		EDCollision::Aabb localAabb;

		std::vector<EDMath::Float3> m_X;
		std::vector<EDMath::Float3> m_OldX;
		std::vector<EDMath::Float3> m_a;
		std::vector<EdgeConstraint> internalConstraints;

		void ApplyVerlet(void);
		void ApplyConstraints(void);
		void ApplyGravity(void);
		void Initialize(void);
	};
}