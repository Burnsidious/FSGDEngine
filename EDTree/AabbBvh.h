#pragma once

#include "EDTree.h"

#include "../EDCollision/CollisionLibrary.h"
#include "../EDMath/Float3.h"

struct SourceData
{
	const EDMath::Float3* pointsPtr;
	EDMath::Float3* centroidsPtr;
	const unsigned int* indiciesPtr;
	unsigned int maxLeafSize;
};

class TiXmlElement;

class AabbBvh : public EDTree
{
protected:
	void* sourceDataPtr;

public:
	AabbBvh(void){}
	AabbBvh(const EDMath::Float3 *pointsPtr, const unsigned int *indicesPtr, unsigned int numIndices, unsigned int maxLeafSize);
	const void* GetSourceData(void)const{ return sourceDataPtr; }
	virtual ~AabbBvh(){}
	void BuildTree(const EDMath::Float3* pointsPtr, const unsigned int* indicesPtr, unsigned int numIndices, unsigned int maxLeafSize);
	static AabbBvh* LoadXML(const char* fileName);
	static AabbBvh* LoadXML(TiXmlElement* elementPtr);

	void GetLocalAabb(EDCollision::Aabb& aabb) const;
};