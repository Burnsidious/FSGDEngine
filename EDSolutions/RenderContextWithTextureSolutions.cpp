#include "precompiled.h"

#include "../EDRendererD3D/RenderContextWithTexture.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/IndexBuffer.h"
#include "../EDRendererD3D/RenderMaterial.h"
#include "../EDUtilities/InternalOutput.h"
using EDUtilities::InternalOutput;

#include "../EDUtilities/PrintConsole.h"
using EDUtilities::PrintConsole;
#include "../EDMath/Float3.h"
using EDMath::Float3;

namespace EDRendererD3D
{
	void RenderContextWithTexture::ContextWithTextureSharedRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderContextWithTexture::ContextWithTextureSharedRenderFuncSolution");
			added = true;
		}

		//RenderContextWithTexture &context = (RenderContextWithTexture &)node;
		//ID3DXEffect	*effectPtr = context.GetEffect();
		//effectPtr->SetTechnique( context.GetRenderStageTechnique() );

		//context.GetEffect()->Begin(&context.GetPasses(), 0);

		//if(context.renderMaterialPtr->GetRTHandles()[RTT_Ambient].GetContent())
		//	context.EffectSetAmbientMap(
		//	context.renderMaterialPtr->GetRTHandles()[RTT_Ambient].GetContent()->GetHandle());
		//if(context.renderMaterialPtr->GetRTHandles()[RTT_Diffuse].GetContent())
		//	context.EffectSetDiffuseMap(
		//	context.renderMaterialPtr->GetRTHandles()[RTT_Diffuse].GetContent()->GetHandle());
		//if(context.renderMaterialPtr->GetRTHandles()[RTT_Specular].GetContent())
		//	context.EffectSetSpecularMap(
		//	context.renderMaterialPtr->GetRTHandles()[RTT_Specular].GetContent()->GetHandle());
		//if(context.renderMaterialPtr->GetRTHandles()[RTT_Emmisive].GetContent())
		//	context.EffectSetEmissiveMap(
		//	context.renderMaterialPtr->GetRTHandles()[RTT_Emmisive].GetContent()->GetHandle());
		//if(context.renderMaterialPtr->GetRTHandles()[RTT_Normal].GetContent())
		//	context.EffectSetNormalMap(
		//	context.renderMaterialPtr->GetRTHandles()[RTT_Normal].GetContent()->GetHandle());
		//if(context.renderMaterialPtr->GetRTCubeMapHandle().GetContent())
		//	context.EffectSetReflectedMap(
		//	context.renderMaterialPtr->GetRTCubeMapHandle().GetContent()->GetHandle());

		//Renderer::Render(context.GetRenderSet());

		//context.GetEffect()->End();
	}

	void RenderContextWithTexture::ContextWithTexturePosNormalUVRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderContextWithTexture::ContextWithTexturePosNormalUVRenderFuncSolution");
			added = true;
		}

		//IDirect3DDevice9 *pDevice = Renderer::theDevicePtr;

		//pDevice->SetStreamSource(0, 
		//	VertexBufferManager::GetReference().GetPosNormTexBuffer().GetVertexBuffer(),
		//	0, sizeof(VERTEX_POSNORMTEX));
		//pDevice->SetVertexDeclaration(
		//	VertexBufferManager::GetReference().GetPosNormTexBuffer().GetVertexDeclaration());

		//pDevice->SetIndices(IndexBuffer::GetReference().GetIndices());

		//ContextSharedRenderFunc(node);
	}

	void RenderContextWithTexture::ContextWithTexturePositionRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderContextWithTexture::ContextWithTexturePositionRenderFuncSolution");
			added = true;
		}

		//IDirect3DDevice9 *pDevice = Renderer::theDevicePtr;

		//pDevice->SetStreamSource(0, 
		//	VertexBufferManager::GetReference().GetPositionBuffer().GetVertexBuffer(),
		//	0, sizeof(VERTEX_POS));
		//pDevice->SetVertexDeclaration(
		//	VertexBufferManager::GetReference().GetPositionBuffer().GetVertexDeclaration());

		//pDevice->SetIndices(IndexBuffer::GetReference().GetIndices());

		//ContextSharedRenderFunc(node);
	}

	void RenderContextWithTexture::ContextWithTexturePositionUVRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderContextWithTexture::ContextWithTexturePositionUVRenderFuncSolution");
			added = true;
		}

		//IDirect3DDevice9 *pDevice = Renderer::theDevicePtr;

		//pDevice->SetStreamSource(0, 
		//	VertexBufferManager::GetReference().GetPositionTexBuffer().GetVertexBuffer(),
		//	0, sizeof(VERTEX_POSTEX));
		//pDevice->SetVertexDeclaration(
		//	VertexBufferManager::GetReference().GetPositionTexBuffer().GetVertexDeclaration());

		//pDevice->SetIndices(IndexBuffer::GetReference().GetIndices());

		//ContextSharedRenderFunc(node);
	}

	void RenderContextWithTexture::ContextPosBoneWeightNormalUVRenderFuncSolution(RenderNode &node)
	{

	}
	void RenderContextWithTexture::ContextPosBoneWeightNormalTanUVRenderFuncSolution(RenderNode &node)
	{

	}
}
