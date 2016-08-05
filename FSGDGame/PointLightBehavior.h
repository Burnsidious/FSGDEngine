#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/PointLightWithShadow.h"

namespace Behaviors
{
	class PointLightBehavior : public EDGOCA::BehaviorT<PointLightBehavior>
	{
	public:
		PointLightBehavior(void);
		~PointLightBehavior(void);

		void OnAdd(void);

		const EDGOCA::AttributeNameKey* GetRequiredAttributes(void);

		static void PopulateMessageMap(void);
		static void OnFrustumTest( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg );
		static void OnBuildLocalAabb( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );
		static void OnGetRenderableCount( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );
		static void OnAttributeChanged( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );
	private:
		EDRendererD3D::PointLightWithShadow* lightPtr;
	};
}