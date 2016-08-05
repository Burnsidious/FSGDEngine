#include "precompiled.h"
#include "RenderSetSorted.h"
#include "RenderContext.h"
#include "RenderShape.h"

namespace EDRendererD3D
{
	void RenderSetSorted::AddRenderNode(RenderNode *nodePtr)
	{
        // Implement your own solution for the sorting and hierarchy labs
        return AddRenderNodeSolution(nodePtr);
	}

	bool RenderSetSorted::ZSortGreater(RenderNode *left, RenderNode *right)
	{
		return ((RenderShape*)left)->GetZDepth() > ((RenderShape*)right)->GetZDepth();
	}

	bool RenderSetSorted::ZSortSmaller(RenderNode *left, RenderNode *right)
	{
		return ((RenderShape*)left)->GetZDepth() < ((RenderShape*)right)->GetZDepth();
	}

	bool RenderSetSorted::SortReturnTrue(RenderNode *left, RenderNode *right)
	{
		return true;
	}

	bool RenderSetSorted::SortReturnFalse(RenderNode *left, RenderNode *right)
	{
		return false;
	}
}