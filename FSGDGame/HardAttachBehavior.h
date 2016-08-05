#pragma once

#include "../EDGOCA/BehaviorT.h"

namespace Behaviors
{
	class HardAttachBehavior : public EDGOCA::BehaviorT<HardAttachBehavior>
	{
	public:
		HardAttachBehavior(void) {}
		~HardAttachBehavior(void) {}

		static void PopulateMessageMap(void);
		static void OnAttach(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
		static void OnSetOffset(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
	};
}