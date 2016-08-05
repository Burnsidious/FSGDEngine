#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDMath/Float3.h"

namespace Behaviors
{
	class CharacterController : public EDGOCA::BehaviorT<CharacterController>
	{
	public:

		CharacterController(void)
		{
			baseSpeed = 7.5f;
			baseRotSpeed = 4.0f;
		}

		void OnAdd(void);

		const EDGOCA::AttributeNameKey* GetRequiredAttributes(void);

		static void PopulateMessageMap(void);

		static void OnMoveForward( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveBackward( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveLeft( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnMoveRight( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnTurnLeft( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnTurnRight( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnMoveUp( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
	private:
		float baseSpeed;
		float baseRotSpeed;

		float ComputeSpeed();
		float ComputeRotSpeed();
	};
}