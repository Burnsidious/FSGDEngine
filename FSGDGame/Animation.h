#pragma once

#include "AnimatedBone.h"

class Animation
{
public:
	Animation(void);
	~Animation(void);

	/// Creates a new indexed mesh in heap memory based on a file.
	/// \param fileName - The name of the file to read the mesh from.
	/// \param pVertexFormat - A string denoting the vertex format to be used, must exist
	/// in the array defined in VertexFormats.cpp.
	/// \return Returns the mesh.  This mesh will need to be deleted.
	static Animation* Load(const char *fileName, const char *ignored);

	/// Processes an instance of animation
	/// \param boneObjects - The GameObjects that should be set to the current animation transform
	/// \param animTime - The time of the animation to set the boneObjects to
	void Process(std::vector<EDGOCA::GameObject> &boneObjects, float animTime);

	float GetDuration() { return duration; }
	unsigned int GetNumBones() { return numBones; }

	std::vector<AnimatedBone> &GetBones(void) { return bones; }


private:
	float duration;
	unsigned int numBones;
	std::vector<AnimatedBone> bones;

	void AddKeyFrames(AnimatedBone &bone, std::vector<KeyFrame> &fileKeyFrames);

};

