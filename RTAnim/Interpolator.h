#pragma once

#include <vector>

#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;

#include "Clip.h"

namespace RTAnim
{
	class InterpolatedBone
	{
		friend class Interpolator;
	public:
		InterpolatedBone(void) : currentKeyFrame(0), bone(nullptr) {}

		const EDMath::Float4x4& GetLocalTransform(void)const { return local; }
		const AnimatedBone* GetAnimatedBone(void)const { return bone; }

	private:
		void Update(float animTime, float duration);

		EDMath::Float4x4 local;
		int currentKeyFrame;
		const AnimatedBone* bone;
	};

	class Interpolator
	{
	public:
		Interpolator(void) : animTime(0.0f), clipPtr(nullptr) {}

		void SetClip(const char* filePath);
		void SetClip(EDUtilities::ContentHandle<Clip> handle);
		
		void SetTime(float t);

		void AddTime(float delta);

		const std::vector<InterpolatedBone>& GetInterpolatedBones(void) const { return interpolatedBones; }

		EDUtilities::ContentHandle<Clip> GetClip(void){ return clipHandle; }
		const Clip* GetClip(void) const { return clipPtr; }

		void Process(void);

	private:
		bool outdated;
		float animTime;
		
		EDUtilities::ContentHandle<Clip> clipHandle;
		Clip* clipPtr;

		std::vector<InterpolatedBone> interpolatedBones;
	};
}