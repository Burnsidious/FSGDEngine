#include "Interpolator.h"

#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;

namespace RTAnim
{
	void InterpolatedBone::Update(float animTime, float duration)
	{
		float startFrameTime = 0.0f, endFrameTime = 0.0f;

		auto& keyFrames = bone->GetKeyFrames();
		size_t numKeyFrames = keyFrames.size();

		unsigned int nextFrameIndex = -1;

		for(size_t count = 0; count < numKeyFrames; ++count)
		{
			startFrameTime = keyFrames[currentKeyFrame].GetTime();

			nextFrameIndex = wrapValue(
				static_cast<unsigned int>(currentKeyFrame + 1), 0u, 
				static_cast<unsigned int>(numKeyFrames));

			endFrameTime = keyFrames[nextFrameIndex].GetTime();

			// Did we wrap the animation?
			if(endFrameTime < startFrameTime)
				startFrameTime = duration;

			// Have we found the correct frame?
			if(startFrameTime < animTime && endFrameTime > animTime)
				break;

			// Test next frame
			currentKeyFrame = nextFrameIndex;
		}

		float delta = (animTime - startFrameTime) / (endFrameTime - startFrameTime);

		local = EDMath::Interpolate( keyFrames[currentKeyFrame].GetLocalTransform(),
										keyFrames[nextFrameIndex].GetLocalTransform(), delta);
	}

	void Interpolator::SetClip(const char* filePath)
	{
		clipHandle = ContentManager::Load<Clip>(filePath);
		clipPtr = clipHandle.GetContent();

		if( clipPtr != 0 )
		{
			if( interpolatedBones.size() != clipPtr->GetNumBones() )
				interpolatedBones.resize( clipPtr->GetNumBones() );
			
			unsigned int boneCount = (unsigned int)interpolatedBones.size();
			auto& clipBones = clipPtr->GetBones();
			for(unsigned int i = 0; i < boneCount; ++i)
			{
				interpolatedBones[i].currentKeyFrame = 0;
				interpolatedBones[i].bone = &clipBones[i];
			}
		}
		else
			interpolatedBones.clear();


		SetTime(0.0f);
	}

	void Interpolator::SetClip(EDUtilities::ContentHandle<Clip> handle)
	{
		clipHandle = handle;
		clipPtr = clipHandle.GetContent();

		if( clipPtr != 0 )
		{
			if( interpolatedBones.size() != clipPtr->GetNumBones() )
				interpolatedBones.resize( clipPtr->GetNumBones() );
			
			unsigned int boneCount = (unsigned int)interpolatedBones.size();
			auto& clipBones = clipPtr->GetBones();
			for(unsigned int i = 0; i < boneCount; ++i)
			{
				interpolatedBones[i].currentKeyFrame = 0;
				interpolatedBones[i].bone = &clipBones[i];
			}
		}
		else
			interpolatedBones.clear();


		SetTime(0.0f);
	}
		
	void Interpolator::SetTime(float t)
	{
		animTime = t;
		outdated = true;
	}

	void Interpolator::AddTime(float delta)
	{
		animTime += delta;
		outdated = true;
	}

	void Interpolator::Process(void)
	{
		if( !outdated )
			return;

		outdated = false;

		if( clipPtr == 0 )
			return;

		animTime = wrapValue( animTime, 0.0f, clipPtr->GetDuration() );

		unsigned int boneCount = (unsigned int)interpolatedBones.size();
		for(unsigned int i = 0; i < boneCount; ++i)
			interpolatedBones[i].Update( animTime, clipPtr->GetDuration() );
	}
}