#pragma once

#include "../EDGOCA/BehaviorT.h"

namespace Behaviors
{
	class ShootingBehavior : public EDGOCA::BehaviorT<ShootingBehavior>
	{
	public:
		const EDGOCA::AttributeNameKey* GetRequiredAttributes(void);

		static void PopulateMessageMap(void);

	    static void OnFire( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);

	private:
	};
}
