#pragma once

#include "AabbBvh.h"

class BvhAabb : public EDCollision::Aabb, public EDTreeInternalNodeData {};

class AabbBvhInternalNode : public EDTreeInternalNode
{
	BvhAabb *AabbPtr;
public:
	~AabbBvhInternalNode(void);

	const BvhAabb* GetInternalNodeData(void) const
	{
		return AabbPtr;
	}

	void SetInternalNodeData(EDTreeInternalNodeData* dataPtr)
	{
		AabbPtr = (BvhAabb*)dataPtr;
	}

	void Traverse( void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, void* traversalDataPtr, void* traversalFuncResultPtr ) ) const;
};