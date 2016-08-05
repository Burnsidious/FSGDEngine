#pragma once

#include "../EDGOCA/BehaviorT.h"

namespace Behaviors
{
	class ProjectileBehavior : public EDGOCA::BehaviorT<ProjectileBehavior>
	{
	public:
		ProjectileBehavior(void)
		{
			baseSpeed = 7.5f;
		}

		~ProjectileBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnUpdate( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnCollide( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );


	private:
		float baseSpeed;

		float ComputeSpeed();
	};
}
