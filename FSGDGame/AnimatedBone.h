#pragma once

#include "../EDMath/float4x4.h"
#include "../EDGOCA/GameObject.h"

// Templated operation which ensures a number is "wrapped" within an upper and lower bound.
// For floating-point numbers "_min" and "_max" are INCLUSIVE.
// For integers the "_max" value is EXCLUSIVE. ex: -1 -> 0-500 = 499
// Example uses:
// Integer: 0, 1, 3 = 2
// Floating-point: -1.2, 0, 1 = 0.8
template< typename T >
static T wrapValue( T _val, T _min, T _max)
{
	// convert whatever it is to doubles then switch back
	double v = double(_val);
	double mn = double(_min);
	double mx = double(_max);
	v -= mn; // determine value distance from start
	// compute ratio of value distance from start to maximum difference
	v = v / (mx-mn) + DBL_EPSILON;
	// convert v to sub-range ratio and invert ratio if negative
	v -= floor(v);
	// v should always be a positive sub-ratio now. scale & cast to proper range & type.
	return T(v * (mx-mn) + mn);
}

class KeyFrame
{
public:
	KeyFrame(float _time, EDMath::Float4x4 _local);

	const EDMath::Float4x4 &GetLocalTransform() { return local; }
	float GetTime() { return time; }
private:
	float time;
	EDMath::Float4x4 local;
};

class AnimatedBone
{
public:
	AnimatedBone(int _parentIndex, const char* _name);
	~AnimatedBone(void);

	void AddKeyFrame(KeyFrame &keyFrame);

	void AddChild(AnimatedBone &childBone);

	void Update(EDGOCA::GameObject &boneObject, float animTime, float duration);

	//const EDMath::Float4x4 &GetWorldTransform() { return boneObject.GetWorldTransform(); }
	const EDMath::Float4x4 &GetBindTransform() { return bindObject.GetWorldTransform(); }

	//EDGOCA::GameObject &GetBoneObject() { return boneObject; }
	EDGOCA::GameObject &GetBindObject() { return bindObject; }

	int GetParentIndex(void) const { return parentIndex; }
private:
	AnimatedBone(void) {}

	bool isBindFrame;
	std::vector<KeyFrame> keyFrames;

	// Used to speed up tracking of keyFrame changes
	int currentKeyFrame;

	int parentIndex;

	std::string name;

	EDGOCA::GameObject bindObject;  
};

