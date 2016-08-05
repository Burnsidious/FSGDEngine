#include "PostProcessChain.h"
#include "WorldData.h"
#include "RenderController.h"

#include "..\EDRendererD3D\DepthStencilStateManager.h"

using namespace EDRendererD3D;

PostProcessChain::PostProcessChain(cbPostProcess &_postShaderData) : postShaderData(_postShaderData)
{
	sourceTargetPtr = destinationTargetPtr = 0;
	postShaderData.gTexCoordScale = 1;
	sampleState = PPT_NoChange;
}

PostProcessChain::~PostProcessChain()
{
}

void PostProcessChain::Initialize()
{
	// Implement your own solution for the Post-process lab
	return InitializeSolution();
}

void PostProcessChain::Run(RenderTarget &mainTarget)
{
	DepthStencilStateManager::GetReference().ApplyState(DepthStencilStateManager::DSS_NoDepth);
	sampleState = PPT_NoChange;
	postShaderData.gTexCoordScale = 1;

	// Implement your own solution for the Post-process lab
	RunSolution(mainTarget);



	// We need the destination of the effect chain to be written to the mainTarget before we return
	RenderContext *sceneContext =  RenderController::GetInstance()->GetScreenContextPtr();
	RenderShapeTarget *screenShape = RenderController::GetInstance()->GetScreenShapePtr();
		
	mainTarget.ActivateTarget();
	screenShape->SetRenderTarget(sourceTargetPtr);
	screenShape->AddToContextSet();
	sceneContext->RenderProcess();
	sceneContext->ClearRenderSet();
	//Renderer::theContextPtr->CopyResource(mainTarget.GetRenderTargetSurface(0)->GetTexture()->GetTextureResource(),
	//	sourceTargetPtr->GetRenderTargetSurface(0)->GetTexture()->GetTextureResource());

	DepthStencilStateManager::GetReference().ApplyState(DepthStencilStateManager::DSS_Default);
}

void PostProcessChain::SwapTargets()
{
	SwapTargetsSolution();
}

void PostProcessChain::UpdateShaderData()
{
	UpdateShaderDataSolution();
}

void PostProcessChain::UpdateViewports(std::list< PostProcessEffect >::iterator &processIter)
{
	UpdateViewportsSolution(processIter);
}

size_t PostProcessChain::RemovePostProcess(EDRendererD3D::PostProcess *effectToRemovePtr)
{
	auto currentPostProcessEffect = processChain.begin();
	auto endPostProcessEffect = processChain.end();

	for(; currentPostProcessEffect != endPostProcessEffect; currentPostProcessEffect++)
	{
		if((*currentPostProcessEffect).postProcessPtr ==  effectToRemovePtr)
		{
			RemovePostProcess(&(*currentPostProcessEffect));
			break;
		}
	}
	return processChain.size();
}
