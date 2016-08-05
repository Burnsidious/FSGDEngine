#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/DirectionalLightWithShadow.h"
#include "../EDRendererD3D/RenderContext.h"

namespace Behaviors
{
	class DirectionalLightController : public EDGOCA::BehaviorT<DirectionalLightController>
	{
	public:
		DirectionalLightController(void) : lightPtr(0) {}
		~DirectionalLightController(void) {}

		static void PopulateMessageMap(void);
		static void OnSetLight( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );
		static void OnSetRenderContext( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );
		static void OnFrustumTest( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );
		static void OnBuildLocalAabb( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );
		static void DirectionalLightController::OnGetRenderableCount( 
			EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );

	private:
		EDRendererD3D::DirectionalLightWithShadow *lightPtr;
		EDRendererD3D::RenderContext *contextPtr;
	};
}
