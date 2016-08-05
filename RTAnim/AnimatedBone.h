#pragma once

#include <vector>
#include <string>

#include "KeyFrame.h"

namespace RTAnim
{
	class AnimatedBone
	{
	public:
		AnimatedBone(int _parentIndex, const char* _name);
		
		//void SetBindTransform(const EDMath::Float4x4& mat);

		void AddKeyFrame(const KeyFrame &keyFrame);

		//const EDMath::Float4x4& GetBindTransform(void)const { return bindTransform; }

		int GetParentIndex(void)const{ return parentIndex; }

		const char* GetName(void)const{ return name.c_str(); }

		unsigned int GetNameHash(void)const{ return nameHash; }

		const std::vector<KeyFrame>& GetKeyFrames(void) const { return keyFrames; }

	private:
		int parentIndex;

		std::string name;
		
		unsigned int nameHash;

		//EDMath::Float4x4 bindTransform;

		std::vector<KeyFrame> keyFrames;
	};
}