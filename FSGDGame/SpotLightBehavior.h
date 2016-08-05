#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/SpotLightWithShadow.h"

namespace Behaviors
{
	class SpotLightBehavior : public EDGOCA::BehaviorT<SpotLightBehavior>
	{
	public:
		SpotLightBehavior(void);
		~SpotLightBehavior(void);

		void OnAdd(void);
		void OnRemove(void);

		static void PopulateMessageMap(void);
		static void OnFrustumTest( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );
		static void OnBuildLocalAabb( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );
		static void OnGetRenderableCount( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );
	private:
		EDRendererD3D::SpotLightWithShadow* lightPtr;
	};
}