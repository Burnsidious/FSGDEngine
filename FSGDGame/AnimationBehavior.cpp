#include "AnimationBehavior.h"

#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
#include "../EDUtilities/XMLLoadingMethods.h"
#include "../EDUtilities/ContentManager.h"

#include "WorldData.h"

using namespace EDGOCA;
using namespace EDUtilities;
using std::string;

namespace AnimationBehaviorMessages
{
	DeclareMessage( MSG_LOAD_ANIMATION);
	DeclareMessage( MSG_UPDATE);
	DeclareMessage( MSG_ANIMATION_UPDATE);
}

namespace Behaviors
{
	AnimationBehavior::AnimationBehavior(void)
	{
		animTime = 0.0f;
	}

	AnimationBehavior::~AnimationBehavior(void)	{ }

	void AnimationBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( AnimationBehaviorMessages::MSG_LOAD_ANIMATION, 
			OnLoadAnimation ) );
		AddMessageHandler( MessageHandler( AnimationBehaviorMessages::MSG_UPDATE, OnUpdate ) );
	}

	void AnimationBehavior::OnUpdate(EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg)
	{
		AnimationBehavior *behaviorPtr = (AnimationBehavior*)pInvokingBehavior;
		
		Animation *animationPtr = behaviorPtr->animPtr.GetContent();

		// Set the time for animation interpolation
		behaviorPtr->animTime += WorldData::GetDeltaTime();
		behaviorPtr->animTime = wrapValue(behaviorPtr->animTime, 0.0f, animationPtr->GetDuration());

		// Process the animation for the given time
		animationPtr->Process(behaviorPtr->boneObjects, behaviorPtr->animTime);

		// Ensure our container is large enough to hold the animation data
		unsigned int numBones = animationPtr->GetNumBones();
		if(behaviorPtr->flatMats.size() < numBones)
			behaviorPtr->flatMats.resize(numBones);

		// Flatten the bind pose and current animation joint matrices
		for(unsigned int boneIndex = 0; boneIndex < numBones; ++boneIndex)
		{
			EDMath::Multiply(behaviorPtr->flatMats[boneIndex], 
				animationPtr->GetBones()[boneIndex].GetBindTransform(),
				behaviorPtr->boneObjects[boneIndex].GetWorldTransform());
		}

		// Tell the renderer we have new animation data to be used
		behaviorPtr->gameObject->OnMessage(
			&MessageT<std::vector<EDMath::Float4x4> &>( 
			AnimationBehaviorMessages::MSG_ANIMATION_UPDATE, behaviorPtr->flatMats));
	}

	void AnimationBehavior::OnLoadAnimation(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		// Interpret the message sent
		AnimationBehavior *behaviorPtr = (AnimationBehavior*)pInvokingBehavior;
		string &fileName = (string &)((MessageT<string> *)msg)->value;

		// Load animation data from file
		behaviorPtr->animPtr = 
			ContentManager::GetInstance()->Load<Animation>(fileName.c_str(), nullptr);

		// Build the skeletal hierarchy
		behaviorPtr->BuildSkeleton();
	}

	void AnimationBehavior::BuildSkeleton(void)
	{
		// Create the GameObjects that represent the bones
		unsigned int numBones = animPtr.GetContent()->GetNumBones();
		boneObjects.resize(numBones);

		const std::vector<AnimatedBone> &animatedBones = animPtr.GetContent()->GetBones();

		// Set parent bones for each bone
		for(unsigned int boneIndex = 0; boneIndex < numBones; ++boneIndex)
		{
			int parentIndex = animatedBones[boneIndex].GetParentIndex();

			if(-1 != parentIndex)
			{
				boneObjects[parentIndex].AddChildObject(&boneObjects[boneIndex], -1);
			}
			else
			{
				gameObject->AddChildObject(&boneObjects[boneIndex], -1);
			}
		}
	}

}
