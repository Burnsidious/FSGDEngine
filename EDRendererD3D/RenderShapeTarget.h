#pragma once

#include "RenderShapeBase.h"
#include "RenderTarget.h"
#include "RenderContext.h"
#include "RenderMesh.h"
#include "../EDMath/float4x4.h"

namespace EDRendererD3D
{
	/// RenderShapeTarget is a specialization of RenderNode used to define 
	/// a renderable using a RenderTarget as its texture.
	class RenderShapeTarget : public RenderShapeBase
	{
	private:
		/// Pointer to the RenderContext of this shape
		RenderContext *renderContextPtr;
		/// Points to the RenderMesh of this shape
		RenderMesh *renderMeshPtr;
		/// Points to the RenderTarget to be used as the texture
		RenderTarget *renderTargetPtr;
		UINT renderTargetIndex;
		DirectX::XMFLOAT4X4 worldMatrix;
	public:
		RenderShapeTarget() : renderContextPtr(0), renderMeshPtr(0), renderTargetPtr(0)
		{
			renderTargetIndex = 0;
		}

		/// Initializes the RenderShape.
		/// \param _renderMeshPtr - The address of the mesh to be used while rendering this shape.
		/// \param _renderContextPtr - The Address of the context to be used while rendering this shape.
		/// \param _renderTargetPtr - The RenderTarget whose texture should be rendered
		void Initialize(RenderMesh *_renderMeshPtr, RenderContext *_renderContextPtr, RenderTarget *_renderTargetPtr);

		/// A helper function to simplify adding shapes to their RenderSets.
		/// This method will add the shape to the RenderSet held by the shapes context.
		/// This is the way to add this shape to a render batch.
		void AddToContextSet(RenderContext *contextPtr = nullptr);

		inline void SetRenderTarget(RenderTarget *_renderTargetPtr, UINT _renderTargetIndex = 0)
		{ 
			renderTargetPtr = _renderTargetPtr; 
			renderTargetIndex = _renderTargetIndex;
		}

		inline RenderMesh *GetRenderMesh(int lod = -1) const
		{
			return renderMeshPtr;
		}

		inline DirectX::XMFLOAT4X4 *GetWorldMatrixPtr() { return &worldMatrix; }
		inline RenderContext *GetRenderContext(int lod = -1) { return renderContextPtr; }
		inline DirectX::XMFLOAT4X4 &GetWorldMatrix() { return worldMatrix; }
		inline void SetWorldMatrix(const DirectX::XMFLOAT4X4 *mat) { worldMatrix = *mat; }
		inline void SetWorldMatrix(const EDMath::Float4x4 *mat) { SetWorldMatrix((DirectX::XMFLOAT4X4 *)mat); }

		/// An EDRenderFunc that can render shapes whose vertices are indexed, 
		/// no transform matrix will be set.  This is typically used for 2D rendering.
		static void IndexedPrimitiveNoTransformRenderFunc(RenderNode &node);

	};
}
