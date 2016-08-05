#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/Light.h"
#include "../EDRendererD3D/RenderContext.h"

namespace Behaviors
{
	class LightController : public EDGOCA::BehaviorT<LightController>
	{
	public:
		LightController(void) : lightPtr(0) {}
		~LightController(void) {}

		static void PopulateMessageMap(void);
		static void OnSetLight( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );
		static void OnSetRenderContext( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );
		static void OnFrustumTest( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );
		static void OnBuildLocalAabb( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );
	private:
		EDRendererD3D::ILight *lightPtr;
		EDRendererD3D::RenderContext *contextPtr;
	};
}
