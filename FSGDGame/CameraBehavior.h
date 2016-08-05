#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDMath/Float4x4.h"

namespace Behaviors 
{
	class CameraBehavior : public EDGOCA::BehaviorT<CameraBehavior>
	{
	public:
		CameraBehavior(void) {}
		~CameraBehavior(void) {}

		static void PopulateMessageMap(void);
		static void OnSetViewPortIndex(EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg);
		static void OnBuildProjection(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
		static void OnUpdate(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);

		static void OnUpdateSolution(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);

	private:
		unsigned int viewPortIndex;
		EDMath::Float4x4 projection;
		float nearClip;
		float farClip;
	};
}
