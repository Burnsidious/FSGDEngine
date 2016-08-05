#pragma once

#include "../EDGOCA/BehaviorT.h"

namespace Behaviors
{
	class SpinBehavior : public EDGOCA::BehaviorT<SpinBehavior>
	{
	public:
		SpinBehavior(void)
		{
		    speed = 1.0f;
		}

		~SpinBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnUpdate( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnSetMatrix( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
	private:
		float speed;
	};
}
