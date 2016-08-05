#include "GDAnimationExportCmd.h"
#include "GDExporter.h"

#include <maya/MSyntax.h>
#include <maya/MItDag.h>
#include <maya/MFnTransform.h>
#include <maya/MAnimUtil.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MAnimControl.h>
#include <maya/MPlugArray.h>
#include <maya/MObjectArray.h>

GDAnimationExportCmd::GDAnimationExportCmd(void)
{
}


GDAnimationExportCmd::~GDAnimationExportCmd(void)
{
}

void *GDAnimationExportCmd::creator(void)
{
	return new GDAnimationExportCmd;
}

MSyntax GDAnimationExportCmd::newSyntax(void)
{
	MSyntax syntax;

	return syntax;
}

MStatus GDAnimationExportCmd::doIt(const MArgList &args)
{
	const char* exportPath = GDExporter::GetInstance()->EstablishExportPath();

	if( exportPath == 0 )
	{
		MGlobal::displayError(MString("GDAnimationExport - No export path established."));
		return MS::kFailure;
	}

	MStatus status;
	MItDag jointIter = MItDag(MItDag::kDepthFirst, MFn::kJoint, &status);
	MDagPath jointPath;

	bool foundRoot = false;
	masterJointList.clear();

	// while JointIter hasn't reached the final bone, move on to the next one:
	while(!jointIter.isDone())
	{
		GDJoint thisJoint;
		jointIter.getPath(jointPath);
		thisJoint.name = jointPath.partialPathName();
		thisJoint.local = MFnTransform(jointPath).transformationMatrix();

		if(!foundRoot)
		{
			MFnDagNode Node(jointPath);
			if(Node.parentCount() == 0)
			{
				foundRoot = true;
				rootPath = jointPath;
			}
			else
			{
				for(unsigned int parentIndex = 0; parentIndex < Node.parentCount(); ++parentIndex)
				{
					if(Node.parent(parentIndex).apiType() == MFn::kJoint)
						break;
					else if(parentIndex == Node.parentCount() - 1)
					{
						foundRoot = true;
						rootPath = jointPath;
					}
				}
			}
		}

		if(MAnimUtil::isAnimated(jointPath))
		{
			// This bone is animated; Gather its data.
			MPlugArray myPlugArray;
			MAnimUtil::findAnimatedPlugs(jointPath, myPlugArray);
			unsigned int mpaLength = myPlugArray.length();

			for(unsigned int nPlugs = 0; nPlugs < mpaLength; ++nPlugs)
			{
				MObjectArray myObjectArray;
				MAnimUtil::findAnimation(myPlugArray[nPlugs], myObjectArray);
				unsigned int moaLength = myObjectArray.length();

				for(unsigned int nObjs = 0; nObjs < moaLength; ++nObjs)
				{
					MFnAnimCurve myAnimCurve(myObjectArray[nObjs]);

					unsigned int macKeyframes = myAnimCurve.numKeyframes();
					for(unsigned int nFrame = 0; nFrame < macKeyframes; ++nFrame)
					{
						MTime Timer(myAnimCurve.time(nFrame));
						GDKeyFrame thisKeyFrame;
						thisKeyFrame.keyTime = Timer;
						thisKeyFrame.timeInSeconds = Timer.as(MTime::kSeconds);

						InsertKeyFrame(thisJoint.keyFrames, thisKeyFrame);
					}
				}
			}
		}

		MAnimControl myAnimControl;
		MTime myAnimControlTime(myAnimControl.currentTime());
		size_t jtKeyFrames = thisJoint.keyFrames.size();
		for(unsigned int keyIndex = 0; keyIndex < jtKeyFrames; ++keyIndex)
		{
			MAnimControl::setCurrentTime(thisJoint.keyFrames[keyIndex].keyTime);
			MFnTransform transform(jointPath);
			thisJoint.keyFrames[keyIndex].local = transform.transformationMatrix();
			thisJoint.keyFrames[keyIndex].world = jointPath.inclusiveMatrix();
		}
		myAnimControl.setCurrentTime(myAnimControlTime);

		masterJointList.push_back(thisJoint);
		jointIter.next();
	}

	GDSkeletonNode rootNode;
	BuildSkeletonHierarchy(&rootNode, rootPath);

	WriteToXML(exportPath, rootNode);
	WriteToBin(exportPath, rootNode);

	return MS::kSuccess;
}

void GDAnimationExportCmd::InsertKeyFrame(std::vector<GDKeyFrame> &keys, GDKeyFrame keyToAdd)
{
	std::vector<GDKeyFrame>::iterator keyIt = keys.begin();

	for( ; keyIt != keys.end(); keyIt++ )
	{
		//if these keys are the same then return
		if( (*keyIt).keyTime == keyToAdd.keyTime )
			return;

		//if it is less than us, add it here and return
		if( keyToAdd.keyTime < (*keyIt).keyTime )
		{
			keys.insert(keyIt, keyToAdd);
			return;
		}
	}
	//if we got all the way through then it goes at the end
	keys.push_back( keyToAdd );
}

bool GDAnimationExportCmd::BuildSkeletonHierarchy(GDSkeletonNode *currentNode, MDagPath &jointPath)
{
	for(unsigned int i = 0; i < masterJointList.size(); ++i)
	{
		if(jointPath.partialPathName() == masterJointList[i].name)
		{
			currentNode->jointIndex = i;
			break;
		}
		if (i + 1 == masterJointList.size())
			return false;
	}
	if(jointPath.childCount())
	{
		for(unsigned int j = 0; j < jointPath.childCount(); ++j)
		{
			if(jointPath.child(j).apiType() == MFn::kJoint)
			{
				GDSkeletonNode nextNode;
				MDagPath ChildPath;
				MFnDagNode Node(jointPath.child(j));
				Node.getPath(ChildPath);
				BuildSkeletonHierarchy(&nextNode, ChildPath);
				currentNode->children.push_back(nextNode);
			}
		}
	}
	else
		return false;

	return true;
}

void GDAnimationExportCmd::WriteToXML(std::string exportPath, GDSkeletonNode rootNode)
{
	std::string filePath = exportPath;
	filePath += "/GDAnimation";
	_mkdir( filePath.c_str() );
	
	filePath += "/";
	
	std::string sceneName;
	GDExporter::GetInstance()->GetSceneName(sceneName);
	filePath += sceneName;
	filePath += ".xml";

	std::ofstream fout( filePath.c_str(), 
		std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );

	char XMLProlog[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

	fout << XMLProlog;

	std::string szAnimName = sceneName;

	size_t index = szAnimName.find_last_of("/");

	szAnimName.erase(0, index+1);

	szAnimName.resize(szAnimName.length() - 4);

	double animationDuration = 0.0;
	if(masterJointList.size())
	{
		//First Keyframe(bindpose) will be skipped for each bone, so the duration will be changed to relect that
		double lastFrameTime = 
			masterJointList[0].keyFrames[masterJointList[0].keyFrames.size() - 1].keyTime.as(MTime::kSeconds);
		double secondFrameTime = masterJointList[0].keyFrames[1].keyTime.as(MTime::kSeconds);
		double firstFrameTime = masterJointList[0].keyFrames[0].keyTime.as(MTime::kSeconds);

		animationDuration = lastFrameTime - secondFrameTime; 
	}
	//animation stuff

	//fout << "<Mesh name=\"" << meshName.c_str() << "\">\n";
	fout << "<Animation name=\"" << szAnimName.c_str() << "\">\n";
	fout << "<Duration>" << animationDuration << "</Duration>\n";
	fout << "<NumBones>" << masterJointList.size() << "</NumBones>\n";

	WriteBonesXML(fout, rootNode, -1);

	fout << "</Animation>\n";

	fout.close();
	fout.clear();
}

void GDAnimationExportCmd::WriteToBin(std::string exportPath, GDSkeletonNode rootNode)
{
	std::string filePath = exportPath;
	filePath += "/GDAnimation";
	_mkdir( filePath.c_str() );
	filePath += "/";
	
	std::string sceneName;
	GDExporter::GetInstance()->GetSceneName(sceneName);
	filePath += sceneName;
	filePath += ".bin";

	std::ofstream fout( filePath.c_str(), 
		std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );

	std::string szAnimName = sceneName;

	size_t index = szAnimName.find_last_of("/");

	szAnimName.erase(0, index+1);

	szAnimName.resize(szAnimName.length() - 4);

	float animationDuration = 0.0;
	if(masterJointList.size())
	{
		//First Keyframe(bindpose) will be skipped for each bone, so the duration will be changed to relect that
		double lastFrameTime = 
			masterJointList[0].keyFrames[masterJointList[0].keyFrames.size() - 1].keyTime.as(MTime::kSeconds);
		double secondFrameTime = masterJointList[0].keyFrames[1].keyTime.as(MTime::kSeconds);
		double firstFrameTime = masterJointList[0].keyFrames[0].keyTime.as(MTime::kSeconds);

		animationDuration = (float)(lastFrameTime - secondFrameTime); 
	}
	//animation stuff

	// The length of the animation name variable
	unsigned int length = (unsigned int)szAnimName.length();
	fout.write((char *)&length, sizeof(length));
	
	// The animation name
	fout.write(szAnimName.c_str(), sizeof(char) * length);

	// Animation Duration
	fout.write((char *)&animationDuration, sizeof(animationDuration));

	// number of bones/joints
	length = (unsigned int)masterJointList.size();
	fout.write((char *)&length, sizeof(length));

	WriteBonesBin(fout, rootNode, -1);

	fout.close();
	fout.clear();
}

void GDAnimationExportCmd::WriteBonesXML(std::ofstream &fout, GDSkeletonNode currentNode, int parentIndex)
{
	//GDJoint &currentJoint = masterJointList[currentNode.jointIndex];
	unsigned int currBone = currentNode.jointIndex;

	fout << "<animBone boneIndex=\"" << currBone << "\">\n";
	fout << "<parentBone>" << parentIndex << "</parentBone>\n";
	//fout << "<KeyFrames numKeys=\"" << masterJointList[currBone].keyFrames.size()-1 << "\">\n";
	fout << "<KeyFrames numKeys=\"" << masterJointList[currBone].keyFrames.size() << "\">\n";

	const int bufferSize = 256;
	char buffer[bufferSize];

	//for each keyframe
	//for( size_t currKey = 1; currKey < masterJointList[currBone].keyFrames.size(); ++currKey )
	for( size_t currKey = 0; currKey < masterJointList[currBone].keyFrames.size(); ++currKey )
	{
		////This is the actuall first animation keytime because the first keyframe of the models
		////we are exporting is the bindpose
		double firstKeyTime = masterJointList[currBone].keyFrames[1].keyTime.as(MTime::kSeconds);
		//double firstKeyTime = masterJointList[currBone].keyFrames[0].keyTime.as(MTime::kSeconds);
		
		//fout << "<keyFrame num=\"" << currKey-1 << "\">\n";
		fout << "<keyFrame num=\"" << currKey << "\">\n";
		fout << "<keyTime>" << masterJointList[currBone].keyFrames[currKey].keyTime.as(
			MTime::kSeconds)-firstKeyTime << "</keyTime>\n";

		//matrix at this time
		fout << "<localTransform>\n";

		//x axis
		memset(buffer, 0, bufferSize);
		sprintf_s(buffer, "<XAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></XAxis>\n", 
			masterJointList[currBone].keyFrames[currKey].local[0][0], 
			masterJointList[currBone].keyFrames[currKey].local[0][1], 
			masterJointList[currBone].keyFrames[currKey].local[0][2], 
			masterJointList[currBone].keyFrames[currKey].local[0][3]);
		fout << buffer;

		//y axis
		memset(buffer, 0, bufferSize);
		sprintf_s(buffer, "<YAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></YAxis>\n", 
			masterJointList[currBone].keyFrames[currKey].local[1][0], 
			masterJointList[currBone].keyFrames[currKey].local[1][1], 
			masterJointList[currBone].keyFrames[currKey].local[1][2], 
			masterJointList[currBone].keyFrames[currKey].local[1][3]);
		fout << buffer;

		//z axis
		memset(buffer, 0, bufferSize);
		sprintf_s(buffer, "<ZAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></ZAxis>\n", 
			masterJointList[currBone].keyFrames[currKey].local[2][0], 
			masterJointList[currBone].keyFrames[currKey].local[2][1], 
			masterJointList[currBone].keyFrames[currKey].local[2][2], 
			masterJointList[currBone].keyFrames[currKey].local[2][3]);
		fout << buffer;

		//w axis
		memset(buffer, 0, bufferSize);
		sprintf_s(buffer, "<WAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></WAxis>\n", 
			masterJointList[currBone].keyFrames[currKey].local[3][0], 
			masterJointList[currBone].keyFrames[currKey].local[3][1], 
			masterJointList[currBone].keyFrames[currKey].local[3][2], 
			masterJointList[currBone].keyFrames[currKey].local[3][3]);
		fout << buffer;

		fout << "</localTransform>\n";

		////matrix at this time
		//fout << "<worldTransform>\n";

		////x axis
		//memset(buffer, 0, bufferSize);
		//sprintf_s(buffer, "<XAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></XAxis>\n", 
		//	masterJointList[currBone].keyFrames[currKey].world[0][0], 
		//	masterJointList[currBone].keyFrames[currKey].world[0][1], 
		//	masterJointList[currBone].keyFrames[currKey].world[0][2], 
		//	masterJointList[currBone].keyFrames[currKey].world[0][3]);
		//fout << buffer;

		////y axis
		//memset(buffer, 0, bufferSize);
		//sprintf_s(buffer, "<YAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></YAxis>\n", 
		//	masterJointList[currBone].keyFrames[currKey].world[1][0], 
		//	masterJointList[currBone].keyFrames[currKey].world[1][1], 
		//	masterJointList[currBone].keyFrames[currKey].world[1][2], 
		//	masterJointList[currBone].keyFrames[currKey].world[1][3]);
		//fout << buffer;

		////z axis
		//memset(buffer, 0, bufferSize);
		//sprintf_s(buffer, "<ZAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></ZAxis>\n", 
		//	masterJointList[currBone].keyFrames[currKey].world[2][0], 
		//	masterJointList[currBone].keyFrames[currKey].world[2][1], 
		//	masterJointList[currBone].keyFrames[currKey].world[2][2], 
		//	masterJointList[currBone].keyFrames[currKey].world[2][3]);
		//fout << buffer;

		////w axis
		//memset(buffer, 0, bufferSize);
		//sprintf_s(buffer, "<WAxis><x>%f</x><y>%f</y><z>%f</z><w>%f</w></WAxis>\n", 
		//	masterJointList[currBone].keyFrames[currKey].world[3][0], 
		//	masterJointList[currBone].keyFrames[currKey].world[3][1], 
		//	masterJointList[currBone].keyFrames[currKey].world[3][2], 
		//	masterJointList[currBone].keyFrames[currKey].world[3][3]);
		//fout << buffer;

		//fout << "</worldTransform>\n";

		fout << "</keyFrame>\n";
	}


	fout << "</KeyFrames>\n";
	fout << "</animBone>\n";

	size_t numChildren = currentNode.children.size();
	for(size_t childIndex = 0; childIndex < numChildren; ++childIndex)
	{
		WriteBonesXML(fout, currentNode.children[childIndex], currentNode.jointIndex);
	}
}

void GDAnimationExportCmd::WriteBonesBin(std::ofstream &fout, GDSkeletonNode currentNode, int parentIndex)
{
	//GDJoint &currentJoint = masterJointList[currentNode.jointIndex];
	unsigned int currBone = currentNode.jointIndex;

	// boneIndex
	fout.write((char *)&currBone, sizeof(currBone));
	
	// parent boneIndex (-1 if there is none)
	fout.write((char *)&parentIndex, sizeof(parentIndex));

	// length of name
	unsigned int nameLength = masterJointList[currBone].name.length();
	fout.write((char*)&nameLength, sizeof(nameLength));

	// name
	fout.write( masterJointList[currBone].name.asChar(), nameLength );

	// numKeyframes
	unsigned int length = (unsigned int)masterJointList[currBone].keyFrames.size();
	fout.write((char *)&length, sizeof(length));

	//for each keyframe
	//for( size_t currKey = 1; currKey < masterJointList[currBone].keyFrames.size(); ++currKey )
	for( size_t currKey = 0; currKey < masterJointList[currBone].keyFrames.size(); ++currKey )
	{
		////This is the actuall first animation keytime because the first keyframe of the models
		////we are exporting is the bindpose
		double firstKeyTime = masterJointList[currBone].keyFrames[1].keyTime.as(MTime::kSeconds);
		//double firstKeyTime = masterJointList[currBone].keyFrames[0].keyTime.as(MTime::kSeconds);

		// keyFrame index
		unsigned int uiCurKey = (unsigned int)currKey;
		fout.write((char *)&uiCurKey, sizeof(uiCurKey));

		// key time
		float time = (float)(masterJointList[currBone].keyFrames[currKey].keyTime.as(
			MTime::kSeconds)-firstKeyTime);
		fout.write((char *)&time, sizeof(time));

		MMatrix mat = masterJointList[currBone].keyFrames[currKey].local;

		// Right to left handed transformation
		mat[0][2] = -mat[0][2];
		mat[1][2] = -mat[1][2];
		mat[2][0] = -mat[2][0];
		mat[2][1] = -mat[2][1];

		mat[3][2] = -mat[3][2];


		//matrix at this time
		for(unsigned int axis = 0; axis < 4; ++axis)
		{
			for(unsigned int component = 0; component < 4; ++component)
			{
				//float out = (float)masterJointList[currBone].keyFrames[currKey].local[axis][component];
				float out = (float)mat[axis][component];
				fout.write((char *)&out, sizeof(out));
			}
		}
	}

	size_t numChildren = currentNode.children.size();
	for(size_t childIndex = 0; childIndex < numChildren; ++childIndex)
	{
		WriteBonesBin(fout, currentNode.children[childIndex], currentNode.jointIndex);
	}
}
