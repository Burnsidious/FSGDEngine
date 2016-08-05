#pragma once

#include "../EDGOCA/BehaviorT.h"


namespace Behaviors
{
	class JoystickBehavior : public EDGOCA::BehaviorT<JoystickBehavior>
	{
	public:
		JoystickBehavior(void);
		~JoystickBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnInitialize(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
	    static void OnLeftStick(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
	    static void OnRightStick(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);

	    static void OnInputCallback(unsigned int eventId, void *eventData, void *listener);

	private:
		std::map<unsigned int, EDGOCA::MessageNameKey > events;

		float translateSpeed;
		float rotateSpeed;

		float ComputeTranslateSpeed();
		float ComputeRotateSpeed();
	};
}
