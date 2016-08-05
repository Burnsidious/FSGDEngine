#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "Animation.h"
#include "../EDUtilities/ContentManager.h"


namespace Behaviors
{
	class AnimationBehavior : public EDGOCA::BehaviorT<AnimationBehavior>
	{
	public:
		AnimationBehavior(void);
		~AnimationBehavior(void);

		static void PopulateMessageMap(void);

		static void OnLoadAnimation(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);
		static void OnUpdate(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg);

	private:
		//float duration;
		float animTime;
		//unsigned int numBones;
		std::vector<EDGOCA::GameObject> boneObjects;
		std::vector<EDMath::Float4x4> flatMats;

		EDUtilities::ContentHandle<Animation> animPtr;
		
		void BuildSkeleton(void);
	};
}