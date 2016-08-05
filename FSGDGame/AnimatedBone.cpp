#include "AnimatedBone.h"

#include "../EDRendererD3D/DebugRenderer.h"
using EDRendererD3D::DebugRenderer;

KeyFrame::KeyFrame(float _time, EDMath::Float4x4 _local)
{
	time = _time;
	local = _local;
}

AnimatedBone::AnimatedBone(int _parentIndex, const char* _name)
{
	currentKeyFrame = 0;
	isBindFrame = true;
	parentIndex = _parentIndex;
	name = _name;
}


AnimatedBone::~AnimatedBone(void)
{
}

void AnimatedBone::AddKeyFrame(KeyFrame &keyFrame)
{
	if(isBindFrame) 
	{
		EDMath::Float4x4 mat = keyFrame.GetLocalTransform();

		EDMath::Float3 f3X = mat.XAxis;
		EDMath::Float3 f3Y = mat.YAxis;
		EDMath::Float3 f3Z = mat.ZAxis;
		EDMath::Float3 f3W = mat.WAxis;

		// new W axis is the negative dot of the old W axis with X,Y,Z
		mat.Wx = -f3X.dotProduct(f3W);
		mat.Wy = -f3Y.dotProduct(f3W);
		mat.Wz = -f3Z.dotProduct(f3W);

		// transpose x,y,z axis
		mat.transpose3x3();

		bindObject.SetLocalTransform(mat);
		isBindFrame = false;
	}
	else
	{
		keyFrames.push_back(keyFrame);
	}
}

void AnimatedBone::AddChild(AnimatedBone &childBone)
{
	bindObject.AddChildObject(&childBone.bindObject, -1);
}

void AnimatedBone::Update(EDGOCA::GameObject &boneObject, float animTime, float duration)
{
	float startFrameTime = 0.0f, endFrameTime = 0.0f;

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

	boneObject.SetLocalTransform(EDMath::Interpolate(keyFrames[currentKeyFrame].GetLocalTransform(), 
		keyFrames[nextFrameIndex].GetLocalTransform(), delta));


#if 0
	DebugRenderer::GetInstance()->SetLineWidth(.01f);
	DebugRenderer::GetInstance()->DrawMatrix(boneObject.GetWorldTransform(), 0.25f);
#endif
}

