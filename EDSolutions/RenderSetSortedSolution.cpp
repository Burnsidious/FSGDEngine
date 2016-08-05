#include "precompiled.h"

#include "../EDRendererD3D/RenderSetSorted.h"
#include "../EDRendererD3D/RenderContext.h"
#include "../EDRendererD3D/RenderShape.h"
#include "../EDUtilities/PrintConsole.h"
using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDRendererD3D
{
	void RenderSetSorted::AddRenderNodeSolution(RenderNode *nodePtr)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Sorting & Hierarchy", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderSetSorted::AddRenderNodeSolution");
			added = true;
		}

		RenderShape *pShape = (RenderShape *)nodePtr;
		// Empty list
		if (0 == headPtr)
		{
			tailPtr = pShape;
			tailPtr->SetNext(0);
			headPtr = pShape;
		}
		else
		{
			RenderShape *pPrevious = 0;
			RenderShape *pCurrent = (RenderShape *)headPtr;

			// Check if this node is first
			if(!SortFunc(pCurrent, pShape))
			{
				pPrevious = pCurrent;
				pCurrent = (RenderShape *)pCurrent->GetNext();

				// Find where this node goes
				while(pCurrent && !SortFunc(pCurrent, pShape))
				{
					pPrevious = pCurrent;
					pCurrent = (RenderShape *)pCurrent->GetNext();
				}
				pPrevious->SetNext(pShape);
			}
			else
				headPtr = pShape;
			pShape->SetNext(pCurrent);
			if(!pCurrent)
				tailPtr = pShape;
		}
	}
}