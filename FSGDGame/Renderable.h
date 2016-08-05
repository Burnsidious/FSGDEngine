#pragma once

#include "RenderableBehaviorT.h"
#include "../EDRendererD3D/RenderShapeBase.h"

namespace Behaviors
{
	class Renderable : public RenderableBehaviorT<Renderable>
	{
	public:
		Renderable(void){}
		
		static void PopulateMessageMap(void);

		const EDGOCA::AttributeNameKey* GetRequiredAttributes(void);

		static void OnAttributeChanged( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );
	};
}