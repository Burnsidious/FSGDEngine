#pragma once

#include "../EDGOCA/BehaviorT.h"

namespace Behaviors
{
	class WheelBehavior : public EDGOCA::BehaviorT<WheelBehavior>
	{
	public:
		WheelBehavior(void)
		{
			baseSpeed = 15.0f;
		}

		~WheelBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnMoveForward( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveBackward( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );

	private:
		float baseSpeed;

		float ComputeSpeed();
	};
}
