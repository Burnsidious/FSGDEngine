#include "precompiled.h"
#include "RenderSet.h"
#include "RenderNode.h"

namespace EDRendererD3D
{
	RenderSet::RenderSet(void)
	{
		ClearRenderSet();
	}

	void RenderSet::ClearRenderSet(void)
	{
        // Implement your own solution for the sorting and hierarchy labs
        return ClearRenderSetSolution();
	}

	void RenderSet::AddRenderNode(RenderNode *nodePtr)
	{
        // Implement your own solution for the sorting and hierarchy labs
        return AddRenderNodeSolution(nodePtr);
	}
}