#pragma once

#include "kdTree.h"
#include "kdTreeSplittingPlane.h"

class kdTreeInternalNode : public EDTreeInternalNode
{
	kdTreeSplittingPlane *planePtr;
public:
	const kdTreeSplittingPlane* GetInternalNodeData(void) const
	{
		return planePtr;
	}

	void SetInternalNodeData(EDTreeInternalNodeData* dataPtr)
	{
		planePtr = (kdTreeSplittingPlane*)dataPtr;
	}

	void Traverse( void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, void* traversalDataPtr, void* traversalFuncResultPtr ) ) const;
};