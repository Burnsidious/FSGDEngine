#pragma once

#include "../EDGOCA/BehaviorT.h"


namespace Behaviors
{
	class KeyboardBehavior : public EDGOCA::BehaviorT<KeyboardBehavior>
	{
	public:
		KeyboardBehavior(void);
		~KeyboardBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnInitialize(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);

	    static void OnKeyCallback(unsigned int eventId, void *eventData, void *listener);

	private:
		std::map<unsigned int, EDGOCA::MessageNameKey > events;
	};
}
