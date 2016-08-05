#pragma once

#include "../EDGOCA/BehaviorT.h"

#define TURN_TO_ROTATION_EPSILON		0.025f
#define TURN_TO_ROTATION_SPEED			1.3f

namespace Behaviors
{
	class TurnToBehavior : public EDGOCA::BehaviorT<TurnToBehavior>
	{
	public:
		TurnToBehavior(void) : targetObjPtr(0) {}
		~TurnToBehavior(void) {}

		static void PopulateMessageMap(void);
		static void OnSetTarget(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
		static void OnUpdate(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);

		static void OnUpdateSolution(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);

	private:

		EDGOCA::GameObject *targetObjPtr;
	};
}
