#include "precompiled.h"
#include "RenderContextWithTexture.h"
#include "Renderer.h"
#include "VertexBufferManager.h"
#include "IndexBuffer.h"

using namespace EDUtilities;

namespace EDRendererD3D
{
    void RenderContextWithTexture::ContextSharedRenderFunc(RenderNode &node)
	{
        return ContextWithTextureSharedRenderFuncSolution(node);
	}

	void RenderContextWithTexture::ContextPosNormalUVRenderFunc(RenderNode &node)
	{
        return ContextWithTexturePosNormalUVRenderFuncSolution(node);
	}

    void RenderContextWithTexture::ContextPosNormalTangentUVRenderFunc(RenderNode &node)
	{
  //      IDirect3DDevice9 *pDevice = Renderer::theDevicePtr;
  //      
  //      pDevice->SetStreamSource(0, 
		//	VertexBufferManager::GetReference().GetPosNormTanTexBuffer().GetVertexBuffer(),
		//	0, sizeof(VERTEX_POSNORMTANTEX));
		//pDevice->SetVertexDeclaration(
		//	VertexBufferManager::GetReference().GetPosNormTanTexBuffer().GetVertexDeclaration());

  //      pDevice->SetIndices(IndexBuffer::GetReference().GetIndices());

  //      ContextWithTextureSharedRenderFuncSolution(node);
	}

    void RenderContextWithTexture::ContextPositionRenderFunc(RenderNode &node)
	{
        return ContextWithTexturePositionRenderFuncSolution(node);
	}

    void RenderContextWithTexture::ContextPositionUVRenderFunc(RenderNode &node)
	{
        return ContextWithTexturePositionUVRenderFuncSolution(node);
	}

    void RenderContextWithTexture::ContextPosBoneWeightNormalUVRenderFunc(RenderNode &node)
	{
        return ContextPosBoneWeightNormalUVRenderFuncSolution(node);
	}

    void RenderContextWithTexture::ContextPosBoneWeightNormalTanUVRenderFunc(RenderNode &node)
	{
        return ContextPosBoneWeightNormalTanUVRenderFuncSolution(node);
	}
}