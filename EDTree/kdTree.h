#pragma once

#include "EDTree.h"
#include "kdTreeLeafBounds.h"
#include "kdTreeLeafNode.h"
#include "kdTreeInternalNode.h"
#include "kdTreeSplittingPlane.h"

class kdTree : public EDTree
{
	float minimumExtent;
public:
	//kdTree(float minimumExtent): minimumExtent(minimumExtent){}
	kdTree(void){}
	kdTree(EDTreeLeafNodeData* boundsPtr, float _minimumExtent);

	virtual ~kdTree(void){}

	float GetMinimumExtent(void){ return minimumExtent; }
protected:
	void BuildTree(EDTreeLeafNodeData* boundsPtr, float _minimumExtent);
};



