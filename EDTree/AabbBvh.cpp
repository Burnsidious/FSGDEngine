#include "AabbBvh.h"
#include "AabbBvhLeafNode.h"
#include "../tinyxml/tinyxml.h"

using namespace EDMath;
using namespace EDCollision;

AabbBvh::AabbBvh(const Float3* pointsPtr, const unsigned int* indicesPtr, unsigned int numIndices, unsigned int maxLeafSize)
{
	BuildTree(pointsPtr, indicesPtr, numIndices, maxLeafSize );
}

void AabbBvh::BuildTree(const Float3* pointsPtr, const unsigned int* indicesPtr, unsigned int numIndices, unsigned int maxLeafSize)
{
	unsigned int triCount = numIndices / 3;

	unsigned int* triangleIndicesPtr = new unsigned int[triCount];
	SourceData* dataPtr = new SourceData;
	dataPtr->centroidsPtr = new Float3[triCount*3];

	dataPtr->pointsPtr = pointsPtr;
	dataPtr->indiciesPtr = indicesPtr;

	dataPtr->maxLeafSize = maxLeafSize;

	for(unsigned int triIndex = 0; triIndex < triCount; ++triIndex)
	{
		triangleIndicesPtr[triIndex] = triIndex; 

		dataPtr->centroidsPtr[triIndex] = (pointsPtr[indicesPtr[triIndex*3]] + pointsPtr[indicesPtr[triIndex*3+1]] + pointsPtr[indicesPtr[triIndex*3+2]]) / 3.0f;
	}

	sourceDataPtr = dataPtr;

	EDCollision::Aabb rootAabb;
	rootAabb.Build( pointsPtr, indicesPtr, numIndices );


	AabbBvhLeafNode::AabbBvhLeafData* firstDataPtr = new (AabbBvhLeafNode::AabbBvhLeafData)( triangleIndicesPtr, triCount, rootAabb );

	EDTree::BuildTree(firstDataPtr);

	delete[] triangleIndicesPtr;
}

void AabbBvh::GetLocalAabb(Aabb& aabb) const
{
	AabbBvhInternalNode* root = (AabbBvhInternalNode*)rootNodePtr;
		
	aabb = *root->GetInternalNodeData();
}

AabbBvh* AabbBvh::LoadXML(const char* fileName)
{
	TiXmlDocument doc(fileName);

	if(!doc.LoadFile())
		return 0;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* elementPtr;
	TiXmlHandle hRoot(0);
		
	elementPtr = hDoc.FirstChildElement().Element();
	if( !elementPtr )
		return 0;

	std::string rootName = elementPtr->Value();

	if( strcmp(rootName.c_str(), "GDMesh") != 0 )
		return 0;

	TiXmlElement* childPtr = elementPtr->FirstChildElement("GDMeshAttribute_List");
	if( childPtr == 0 )
		return 0;

	TiXmlElement* attributeElementPtr = childPtr->FirstChildElement("GDMeshAttribute");
	TiXmlElement* meshBVHElementPtr = 0;

	while( attributeElementPtr != 0 )
	{
		TiXmlElement* grandChildPtr = attributeElementPtr->FirstChildElement("MeshBVH_List");
		if( grandChildPtr != 0 )
		{
			meshBVHElementPtr = grandChildPtr;
			break;
		}

		attributeElementPtr = attributeElementPtr->NextSiblingElement("GDMeshAttribute");
		continue;
	}

	if( meshBVHElementPtr != 0 )
		return LoadXML(meshBVHElementPtr);

	return 0;
}

AabbBvh* AabbBvh::LoadXML(TiXmlElement* elementPtr)
{
	//MeshBVH bvh;
	//bvh.ReadXML(elementPtr);
	//return bvh.GetBvh();

	return 0;
}