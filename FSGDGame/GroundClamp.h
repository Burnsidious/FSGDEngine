#pragma once

#include "../EDGOCA/BehaviorT.h"

namespace Behaviors
{
	class GroundClamp : public EDGOCA::BehaviorT<GroundClamp>
	{
		bool isEnabled;
	public:
		GroundClamp(void) : isEnabled(true) {}
		
		const EDGOCA::AttributeNameKey* GetRequiredAttributes(void);

		static void PopulateMessageMap(void);

		static void OnCollide( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnUpdate( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
	};
}