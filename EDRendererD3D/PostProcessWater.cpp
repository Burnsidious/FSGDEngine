#include "precompiled.h"
#include "PostProcessWater.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/IndexBuffer.h"

#include "../EDUtilities/InternalOutput.h"
using EDUtilities::InternalOutput;

namespace EDRendererD3D
{
	PostProcessWater::PostProcessWater(void)
	{
		elapsedTime = 0.0f;
		timer.Reset();
	}

	PostProcessWater::~PostProcessWater(void)
	{

	}

//	ID3DXBuffer *PostProcessWater::Initialize(RenderMesh *_meshPtr, LPCWSTR effectFile)
//	{
//		meshPtr = _meshPtr;
//        ID3DXBuffer *errors;
//
//#ifdef _DEBUG
//        D3DXCreateEffectFromFile(Renderer::theDevicePtr, effectFile, 0, 0, 
//            D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION, Renderer::theEffectPoolPtr,
//            &effectPtr, &errors);
//#else
//        D3DXCreateEffectFromFile(Renderer::theDevicePtr, effectFile, 0, 0, 
//            0, Renderer::theEffectPoolPtr, &effectPtr, &errors);
//#endif
//
//        if (errors)
//            InternalOutput::GetReference() << (char *)errors->GetBufferPointer();
//
//		else
//		{
//			// Initialize handles
//			textureHandle = effectPtr->GetParameterByName(0, "sourceTexture");
//			effectPtr->SetTechnique("PostProcess");
//
//			// Texture handles
//			reflectionMapTextureHandle = effectPtr->GetParameterByName("gReflectionMapTexture", 0);
//			heightMapTextueHandle = effectPtr->GetParameterByName("gHeightMapTexture", 0);
//			noiseTextureHandle = effectPtr->GetParameterByName("gNoiseTexture", 0);
//			foamTextureHandle = effectPtr->GetParameterByName("gFoamTexture", 0);
//			causticTextureHandle = effectPtr->GetParameterByName("gCausticTexture", 0);
//
//			// Color handles
//			surfaceColorHandle = effectPtr->GetParameterByName("gSurfaceColor", 0);
//			ambientColorHandle = effectPtr->GetParameterByName("gAmbientColor", 0);
//
//			// Other handles
//			elapsedTimeHandle = effectPtr->GetParameterByName("gElapsedTime", 0);
//		}
//
//        return errors;
//	}

	void PostProcessWater::LoadXML(const char* xmlFileName)
	{
		EDUtilities::ContentHandle<RenderMaterial> materialHandle = 
			EDUtilities::ContentManager::LoadXML<RenderMaterial>( xmlFileName );

		if(materialHandle.GetContent() != NULL)
			renderMaterialHandle = materialHandle;
	}

	//void PostProcessWater::Process(RenderTarget &destinationTarget, LPDIRECT3DBASETEXTURE9 sourceTexture)
	//{
	//	// Implement your own solution for the Post-process lab
	//	//return ProcessSolution(destinationTarget, sourceTexture);

	//	elapsedTime += timer.GetElapsedTime();

	//	destinationTarget.ActivateTarget(0);
 //       IDirect3DDevice9 *pDevice = Renderer::theDevicePtr;
	//	Renderer::theDevicePtr->Clear(0,0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1, 0); 
 //       
	//	// Set the vertex and index source and declarion
	//	pDevice->SetVertexDeclaration(VertexBufferManager::GetReference().GetPositionTexBuffer().GetVertexDeclaration());
 //       pDevice->SetStreamSource(0, VertexBufferManager::GetReference().GetPositionTexBuffer().GetVertexBuffer(),
	//							 0, sizeof(VERTEX_POSTEX));
 //       pDevice->SetIndices(IndexBuffer::GetReference().GetIndices());

	//	effectPtr->SetTexture(textureHandle, sourceTexture);
	//	effectPtr->SetFloat(elapsedTimeHandle, elapsedTime);

	//	// RTT_Ambient, RTT_Diffuse, RTT_Specular, RTT_Emmisive, RTT_Normal
	//	effectPtr->SetTexture(reflectionMapTextureHandle, renderMaterialHandle.GetContent()->GetRTCubeMapHandle().GetContent()->GetHandle());
	//	effectPtr->SetTexture(heightMapTextueHandle, renderMaterialHandle.GetContent()->GetRTHandles()[RTT_Diffuse].GetContent()->GetHandle());
	//	effectPtr->SetTexture(noiseTextureHandle, renderMaterialHandle.GetContent()->GetRTHandles()[RTT_Normal].GetContent()->GetHandle());
	//	effectPtr->SetTexture(foamTextureHandle, renderMaterialHandle.GetContent()->GetRTHandles()[RTT_Ambient].GetContent()->GetHandle());
	//	effectPtr->SetTexture(causticTextureHandle, renderMaterialHandle.GetContent()->GetRTHandles()[RTT_Specular].GetContent()->GetHandle());
	//	effectPtr->SetValue(surfaceColorHandle, &renderMaterialHandle.GetContent()->GetRenderColors()[RTT_Diffuse], sizeof(RenderColor));
	//	effectPtr->SetValue(ambientColorHandle, &renderMaterialHandle.GetContent()->GetRenderColors()[RTT_Ambient], sizeof(RenderColor));

	//	pDevice->SetVertexDeclaration(VertexBufferManager::GetReference().GetPositionTexBuffer().GetVertexDeclaration());
	//	pDevice->SetStreamSource(0, VertexBufferManager::GetReference().GetPositionTexBuffer().GetVertexBuffer(), 0, sizeof(VERTEX_POSTEX));
	//	pDevice->SetIndices(IndexBuffer::GetReference().GetIndices());

	//	// Begin the post-process effect
	//	UINT passes = 0;
	//	effectPtr->Begin(&passes, 0);

	//	// Iterate through the post-process passes
	//	for(UINT pass = 0; pass < passes; ++pass)
	//	{
	//		effectPtr->BeginPass(pass);

	//		Renderer::theDevicePtr->DrawIndexedPrimitive(meshPtr->GetPrimitiveType(), meshPtr->GetStartVertex(), 0, meshPtr->GetVertCount(),
	//												  meshPtr->GetStartIndex(), meshPtr->GetPrimitiveCount());

	//		effectPtr->EndPass();
	//	}

	//	effectPtr->End();

	//	destinationTarget.RevertTarget();

	//	timer.Reset();
	//}
}