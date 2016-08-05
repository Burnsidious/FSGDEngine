#pragma once
#include "RenderNode.h"

namespace EDRendererD3D
{
    class RenderNode;
	/// Container class defines a render batch.
    class RenderSet
    {
        friend class Renderer;
    protected:
		/// The first item in the render batch.
        RenderNode *headPtr;
		/// The last item in the render batch.
        RenderNode *tailPtr;
    public:
		/// Initializes the head and tail pointers to a default state.
        RenderSet(void);
        virtual ~RenderSet(void) {} 

		/// Adds a RenderNode to the render batch.
		/// \param nodePtr - The RenderNode to add to the render batch
        virtual void AddRenderNode(RenderNode *nodePtr);
		/// Sets the head and tail pointers to a default state.
        void ClearRenderSet(void);
		/// \return Returns pointer to the first item in the render batch.
        RenderNode *GetHead() { return headPtr; }

        void AddRenderNodeSolution(RenderNode *nodePtr);
        void ClearRenderSetSolution(void);
    };
}