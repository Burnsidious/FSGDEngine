#include "Animation.h"

using std::list;
using std::ifstream;
using std::string;
#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;


Animation::Animation(void)
{
}


Animation::~Animation(void)
{
}

Animation* Animation::Load(const char*fileName, const char *ignored)
{
	string temp = ContentManager::theContentPath;
	temp += fileName;

	InternalOutput::GetReference() << "Loading " << temp << "...\n";

	ifstream fin(temp.c_str(), std::ios_base::in | std::ios_base::binary);

	if(!fin.is_open())
		InternalOutput::GetReference() << "Failed to open" << fileName << ", does the file exist\n";

	Animation *newAnim = new Animation;

	// read in length of animation
	unsigned int nameLength;
	fin.read((char *)&nameLength, sizeof(nameLength));

	// read in animation name
	char *animationName = new char[nameLength];
	fin.read(animationName, sizeof(char) * nameLength);

	// read animation duration
	fin.read((char *)&newAnim->duration, sizeof(newAnim->duration));

	// read number of bones
	fin.read((char *)&newAnim->numBones, sizeof(newAnim->numBones));

	// reserve memory for the bones to prevent resizing issues
	newAnim->bones.reserve(newAnim->numBones);

	// for each bone
	for(unsigned int boneIndex = 0; boneIndex < newAnim->numBones; ++boneIndex)
	{
		// read the bone index value from file
		unsigned int fileBoneIndex;
		fin.read((char *)&fileBoneIndex, sizeof(fileBoneIndex));

		// read the bone index value from file
		int parentIndex;
		fin.read((char *)&parentIndex, sizeof(parentIndex));

		unsigned int nameLen;
		fin.read((char*)&nameLen, sizeof(nameLen));

		char* nameBuffer = new char[nameLen+1];
		nameBuffer[nameLen] = 0;
		fin.read( nameBuffer, nameLen );

		// read number of keyframes
		unsigned int numKeyframes;
		fin.read((char *)&numKeyframes, sizeof(numKeyframes));

		// Add a bone to work with
		newAnim->bones.push_back(AnimatedBone(parentIndex, nameBuffer));
		AnimatedBone &bone = newAnim->bones[newAnim->bones.size() -1];

		delete [] nameBuffer;

		std::vector<KeyFrame> fileKeyFrames;
		fileKeyFrames.reserve(numKeyframes);
		// for each keyframe
		for(unsigned int keyFrameIndex = 0; keyFrameIndex < numKeyframes; ++keyFrameIndex )
		{
			// Read in the file key frame index
			unsigned int fileKeyIndex;
			fin.read((char *)&fileKeyIndex, sizeof(fileKeyIndex));

			// Read keyFrame time
			float keyTime;
			fin.read((char *)&keyTime, sizeof(keyTime));

			// Read in the local space matrix
			EDMath::Float4x4 local;
			fin.read((char *)&local, sizeof(local));

			// Build and insert keyframe
			KeyFrame keyFrame(keyTime, local);
			fileKeyFrames.push_back(keyFrame);
			//bone.AddKeyFrame(keyFrame);
		} 
		newAnim->AddKeyFrames(bone, fileKeyFrames);

		// Apply parenting if needed
		if(-1 != parentIndex)
		{
			newAnim->bones[parentIndex].AddChild(newAnim->bones[newAnim->bones.size()-1] );
		}
	}

	delete [] animationName;
	return newAnim;
}

void Animation::AddKeyFrames(AnimatedBone &bone, std::vector<KeyFrame> &fileKeyFrames)
{
	// We should always have one bind pose keyframe, and at least one set of keyframes to interpolate through
	assert(fileKeyFrames.size() > 2);

	// Add the bind pose frame
	bone.AddKeyFrame(fileKeyFrames[0]);

	// Add the first keyFrame
	bone.AddKeyFrame(fileKeyFrames[1]);

	// Test each additional keyframe
	unsigned int prevIndex = 1;
	unsigned int currIndex = 2;
	unsigned int nextIndex = 3;
	size_t numKeyFrames = fileKeyFrames.size();

	while(currIndex < numKeyFrames)
	{
		if(nextIndex >= numKeyFrames)
		{
			bone.AddKeyFrame(fileKeyFrames[currIndex]);
			break;
		}

		float threshold = 0.00265f;//0.0265f;

		float totalTime = fileKeyFrames[nextIndex].GetTime() - fileKeyFrames[prevIndex].GetTime();
		float deltaTime = fileKeyFrames[currIndex].GetTime() - fileKeyFrames[prevIndex].GetTime();
		float delta = deltaTime / totalTime;

		EDMath::Float4x4 interp = EDMath::Interpolate(fileKeyFrames[prevIndex].GetLocalTransform(),
			fileKeyFrames[nextIndex].GetLocalTransform(), delta);

		bool bDifferent = !fileKeyFrames[currIndex].GetLocalTransform().CompareComponents(
			interp, threshold);

		if(bDifferent)
		{
			bone.AddKeyFrame(fileKeyFrames[currIndex]);
		}

		prevIndex = currIndex;
		currIndex = nextIndex;
		++nextIndex;
	}
	
}

void Animation::Process(std::vector<EDGOCA::GameObject> &boneObjects, float animTime)
{
	// The GameObject bone list must have an entry for each bone in the animation
	assert(boneObjects.size() == bones.size());

	for(unsigned int boneIndex = 0; boneIndex < numBones; ++boneIndex)
	{
		bones[boneIndex].Update(boneObjects[boneIndex], animTime, duration);
	}

}
