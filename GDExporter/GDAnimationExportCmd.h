#pragma once

#include <maya/MPxCommand.h>
#include <maya/MMatrix.h>
#include <maya/MTime.h>
#include <maya/MDagPath.h>
#include <vector>

struct GDKeyFrame
{
	double  timeInSeconds;
	MTime	keyTime;
	MMatrix	local;
	MMatrix	world;
};

struct GDJoint
{
	MMatrix	local;
	MString	name;

	//a collection of key frames
	std::vector<GDKeyFrame> keyFrames;
};

struct GDSkeletonNode
{
	unsigned int jointIndex;
	std::vector<GDSkeletonNode> children;
};

class GDAnimationExportCmd : public MPxCommand
{
public:
	GDAnimationExportCmd(void);
	~GDAnimationExportCmd(void);

	static void *creator(void);
	static MSyntax newSyntax(void);
	MStatus doIt(const MArgList &args);

	void WriteBonesXML(std::ofstream &fout, GDSkeletonNode currentNode, int parentIndex);
	void WriteBonesBin(std::ofstream &fout, GDSkeletonNode currentNode, int parentIndex);
private:

	std::vector<GDJoint> masterJointList;
	MDagPath rootPath;

	void InsertKeyFrame(std::vector<GDKeyFrame> &keys, GDKeyFrame keyToAdd);
	bool BuildSkeletonHierarchy(GDSkeletonNode *currentNode, MDagPath &jointPath);
	void WriteToXML(std::string exportPath, GDSkeletonNode rootNode);
	void WriteToBin(std::string exportPath, GDSkeletonNode rootNode);
};

