#pragma once
#include "RenderNode.h"
#include "RenderForm.h"
#include "RenderContext.h"

namespace EDRendererD3D
{
	class RenderShapeBase :
		public RenderNode
	{
	public:
		RenderShapeBase(void) {}
		virtual ~RenderShapeBase(void) = 0 {}
		virtual DirectX::XMFLOAT4X4 *GetWorldMatrixPtr() = 0;
		virtual void SetWorldMatrix(const DirectX::XMFLOAT4X4 *mat) = 0;
		virtual RenderContext *GetRenderContext(int lod = -1) = 0;
		virtual void AddToContextSet(RenderContext *contextPtr = nullptr) = 0;

		virtual void Initialize(
			std::vector< EDUtilities::ContentHandle<RenderForm> > &formHandles, 
			DirectX::XMFLOAT4X4* localMatrixPtr = 0 ) {}

		virtual inline RenderMesh *GetRenderMesh(int lod = -1) const
		{ 
			return 0;
		}
	};
}
