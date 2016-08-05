#pragma once

#include "RenderableSetBehaviorT.h"
#include "../EDRendererD3D/RenderShapeBase.h"

namespace Behaviors
{
	class BaseRenderable : public RenderableSetBehaviorT<BaseRenderable>
	{
	public:
		BaseRenderable(void){}
		
		static void PopulateMessageMap(void);

		const EDGOCA::AttributeNameKey* GetRequiredAttributes(void);

		static void OnAttributeChanged( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );
		
		static void OnAnimationUpdate( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );
	};
}