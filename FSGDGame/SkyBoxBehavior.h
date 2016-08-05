#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/RenderShape.h"

namespace Behaviors
{
	class SkyBoxBehavior : public EDGOCA::BehaviorT<SkyBoxBehavior>
	{
	public:
		SkyBoxBehavior(void){}
		~SkyBoxBehavior() {	}
		
		static void PopulateMessageMap(void);
		static void OnBuildLocalAabb( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
	};
}