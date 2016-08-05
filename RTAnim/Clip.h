#pragma once

#include <vector>
#include "AnimatedBone.h"

namespace RTAnim
{
	class Clip
	{
	public:
		static Clip* Load(const char *fileName, const char *ignored = 0);

		float GetDuration()const { return duration; }
		unsigned int GetNumBones()const { return numBones; }

		const std::vector<AnimatedBone> &GetBones(void)const { return bones; }
		const std::vector<EDMath::Float4x4> &GetBindTransforms(void)const { return bindTransforms; }

	private:
		float duration;

		unsigned int numBones;

		std::vector<AnimatedBone> bones;
		std::vector<EDMath::Float4x4> bindTransforms;

		void AddKeyFrames(AnimatedBone &bone, std::vector<KeyFrame> &fileKeyFrames);
	};
}