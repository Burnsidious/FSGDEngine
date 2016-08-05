#include "AnimatedBone.h"
#include <functional>

namespace RTAnim
{
	AnimatedBone::AnimatedBone(int _parentIndex, const char* _name)
	{
		parentIndex = _parentIndex;
		name = _name;
		nameHash = (unsigned int)std::hash< std::string >()(name);
		//bindTransform.makeIdentity();
	}
	
	void AnimatedBone::AddKeyFrame(const KeyFrame &keyFrame)
	{
		keyFrames.push_back(keyFrame);
	}

	//void AnimatedBone::SetBindTransform(const EDMath::Float4x4& mat)
	//{
	//	bindTransform = mat;

	//	EDMath::Float3 f3X = mat.XAxis;
	//	EDMath::Float3 f3Y = mat.YAxis;
	//	EDMath::Float3 f3Z = mat.ZAxis;
	//	EDMath::Float3 f3W = mat.WAxis;

	//	// new W axis is the negative dot of the old W axis with X,Y,Z
	//	bindTransform.Wx = -f3X.dotProduct(f3W);
	//	bindTransform.Wy = -f3Y.dotProduct(f3W);
	//	bindTransform.Wz = -f3Z.dotProduct(f3W);

	//	// transpose x,y,z axis
	//	bindTransform.transpose3x3();
	//}
}