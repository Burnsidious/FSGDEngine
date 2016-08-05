#include "precompiled.h"
#include "RenderShapeTarget.h"
#include "Renderer.h"
#include "IndexBuffer.h"

using namespace DirectX;

namespace EDRendererD3D
{
	void RenderShapeTarget::Initialize(RenderMesh *_renderMeshPtr, 
        RenderContext *_renderContextPtr, RenderTarget *_renderTargetPtr)
    {
        renderMeshPtr = _renderMeshPtr;
        renderContextPtr = _renderContextPtr;
        renderTargetPtr = _renderTargetPtr;
        RenderFunc = IndexedPrimitiveNoTransformRenderFunc;

		XMMATRIX I = XMMatrixIdentity();
		XMStoreFloat4x4(&worldMatrix, I);
    }

	void RenderShapeTarget::AddToContextSet(RenderContext *contextPtr)
    {
		if(!contextPtr)
			renderContextPtr->AddRenderNode(this);
		else
			contextPtr->AddRenderNode(this);
    }

	void RenderShapeTarget::IndexedPrimitiveNoTransformRenderFunc(RenderNode &node)
    {
		RenderShapeTarget &shape = (RenderShapeTarget &)node;
		RenderMesh &mesh = *shape.GetRenderMesh();
		RenderContext &context = *shape.GetRenderContext();

		context.EffectSetDiffuseMap(
			shape.renderTargetPtr->GetShaderResourceView(shape.renderTargetIndex));

		Renderer::theContextPtr->DrawIndexed(mesh.GetPrimitiveCount() * 3, mesh.GetStartIndex(), mesh.GetStartVertex());

		context.EffectSetDiffuseMap(nullptr);
    }
}