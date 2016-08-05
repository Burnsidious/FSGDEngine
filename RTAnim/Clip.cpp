#include <assert.h>
#include "Clip.h"

#include <string>
using namespace std;

#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;


namespace RTAnim
{
	Clip* Clip::Load(const char *fileName, const char *ignored)
	{
		string temp = ContentManager::theContentPath;
		temp += fileName;

		InternalOutput::GetReference() << "Loading " << temp << "...\n";

		ifstream fin(temp.c_str(), std::ios_base::in | std::ios_base::binary);

		if(!fin.is_open())
			InternalOutput::GetReference() << "Failed to open" << fileName << ", does the file exist\n";

		Clip *newAnim = new Clip;

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

		newAnim->bindTransforms.resize(newAnim->numBones);
		
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

			EDMath::Float4x4 mat = fileKeyFrames[0].GetLocalTransform();
			EDMath::Float4x4 bindTransform = mat;

			EDMath::Float3 f3X = mat.XAxis;
			EDMath::Float3 f3Y = mat.YAxis;
			EDMath::Float3 f3Z = mat.ZAxis;
			EDMath::Float3 f3W = mat.WAxis;

			// new W axis is the negative dot of the old W axis with X,Y,Z
			bindTransform.Wx = -f3X.dotProduct(f3W);
			bindTransform.Wy = -f3Y.dotProduct(f3W);
			bindTransform.Wz = -f3Z.dotProduct(f3W);

			// transpose x,y,z axis
			bindTransform.transpose3x3();

			// Apply parenting if needed
			if(-1 != parentIndex)
				EDMath::Multiply( newAnim->bindTransforms[ fileBoneIndex ], bindTransform, newAnim->bindTransforms[ parentIndex ] );
			else
				newAnim->bindTransforms[ fileBoneIndex ] = bindTransform;			
		}

		delete [] animationName;
		return newAnim;		
	}

	void Clip::AddKeyFrames(AnimatedBone &bone, std::vector<KeyFrame> &fileKeyFrames)
	{
		// We should always have one bind pose keyframe, and at least one set of keyframes to interpolate through
		assert(fileKeyFrames.size() > 2);

		// Add the bind pose frame
		//bone.AddKeyFrame(fileKeyFrames[0]);

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

			float threshold = 0.000265f;//0.0265f;

			float totalTime = fileKeyFrames[nextIndex].GetTime() - bone.GetKeyFrames().back().GetTime(); //fileKeyFrames[prevIndex].GetTime();
			float deltaTime = fileKeyFrames[currIndex].GetTime() - bone.GetKeyFrames().back().GetTime();//fileKeyFrames[prevIndex].GetTime();
			//float totalTime = fileKeyFrames[nextIndex].GetTime() - fileKeyFrames[prevIndex].GetTime();
			//float deltaTime = fileKeyFrames[currIndex].GetTime() - fileKeyFrames[prevIndex].GetTime();
			float delta = deltaTime / totalTime;

			EDMath::Float4x4 interp = EDMath::Interpolate(bone.GetKeyFrames().back().GetLocalTransform()/*fileKeyFrames[prevIndex].GetLocalTransform()*/,
				fileKeyFrames[nextIndex].GetLocalTransform(), delta);

			//EDMath::Float4x4 interp = EDMath::Interpolate(fileKeyFrames[prevIndex].GetLocalTransform(),
				//fileKeyFrames[nextIndex].GetLocalTransform(), delta);

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
}