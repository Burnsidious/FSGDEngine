#pragma once

#include "../EDGOCA/BehaviorT.h"

namespace Behaviors
{
	class TurretBehavior : public EDGOCA::BehaviorT<TurretBehavior>
	{
	public:
		TurretBehavior(void)
		{
			baseSpeed = 15.0f;
		}

		~TurretBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnRotateRight( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message);
		static void OnRotateLeft( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
	    static void OnRightStick( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);

	private:
		float baseSpeed;

		float ComputeRotateSpeed();
	};
}
