#pragma once

#include "../EDGOCA/BehaviorT.h"

// this could be made part of the class to have different mouse speeds per object
#define MOUSE_LOOK_SPEED_MODIFIER			0.0025f

namespace Behaviors
{
	class MouseLookBehavior : public EDGOCA::BehaviorT<MouseLookBehavior>
	{
	public:
		MouseLookBehavior(void)
		{
			rotateEnabled = false;
		}
		~MouseLookBehavior(void) {}

		static void PopulateMessageMap(void);
		static void OnBeginRotate(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
		static void OnEndRotate(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
		static void OnRotate(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
		static void OnInitialize(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
	    static void OnMouseEventCallback(unsigned int eventId, void *eventData, void *listener);

		static void OnRotateSolution(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);

	private:
		long cursorPosX;
		long cursorPosY;
		bool rotateEnabled;
		std::map<unsigned int, EDGOCA::MessageNameKey > events;

	};
}
