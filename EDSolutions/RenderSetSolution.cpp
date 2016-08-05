#include "precompiled.h"

#include "../EDRendererD3D/RenderSet.h"
#include "../EDUtilities/PrintConsole.h"
using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDRendererD3D
{
	void RenderSet::ClearRenderSetSolution(void)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Sorting & Hierarchy", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderSet::ClearRenderSetSolution");
			added = true;
		}

		headPtr = tailPtr = 0;
	}

	void RenderSet::AddRenderNodeSolution(RenderNode *nodePtr)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Sorting & Hierarchy", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderSet::AddRenderNodeSolution");
			added = true;
		}

		nodePtr->SetNext(0);
		// Empty list
		if (0 == headPtr)
		{
			headPtr = nodePtr;
		}
		else
		{
			tailPtr->SetNext(nodePtr);
		}
		tailPtr = nodePtr;
	}
}