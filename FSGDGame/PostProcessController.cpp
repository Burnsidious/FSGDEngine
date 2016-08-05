
#include "PostProcessController.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDUtilities/Settings.h"

using EDRendererD3D::Renderer;
using std::list;

PostProcessController *PostProcessController::instancePtr = nullptr;

PostProcessController::PostProcessController(void)
{
}


PostProcessController::~PostProcessController(void)
{
	Clear();
}

PostProcessController *PostProcessController::GetInstance(void)
{
	if(nullptr == instancePtr)
	{
		instancePtr = new PostProcessController();
	}
	return instancePtr;
}

PostProcessController &PostProcessController::GetReference(void)
{
	return *GetInstance();
}

void PostProcessController::DeleteInstance(void)
{
	delete instancePtr;
}

void PostProcessController::InitializePostEffects(EDRendererD3D::RenderMesh *screenQuadMeshPtr)
{
	edgePostProcess.Initialize(screenQuadMeshPtr, 
		"2DPassThrough_VS.cso", "DrawEdge_PS.cso");
	edgeSobelPostProcess.Initialize(screenQuadMeshPtr, 
		"2DPassThrough_VS.cso", "DrawEdgeSobel_PS.cso");
	edgeSobelNormPostProcess.Initialize(screenQuadMeshPtr, 
		"2DPassThrough_VS.cso", "DrawEdgeNormSobel_PS.cso");
	verticalBlurPostProcess.Initialize(screenQuadMeshPtr, 
		"GaussianBlurV_VS.cso", "GaussianBlur_PS.cso");
	horizontalBlurPostProcess.Initialize(screenQuadMeshPtr, 
		"GaussianBlurH_VS.cso", "GaussianBlur_PS.cso");
	depthOfFieldPostProcess.Initialize(screenQuadMeshPtr, 
		 "2DPassThrough_VS.cso", "DepthOfField_PS.cso");
	passThroughPostProcess.Initialize(screenQuadMeshPtr, 
		"2DPassThrough_VS.cso", "PostPassThrough_PS.cso");
	brightPostProcess.Initialize(screenQuadMeshPtr, 
		"2DPassThrough_VS.cso", "BrightPass_PS.cso");
	verticalBloomPostProcess.Initialize(screenQuadMeshPtr, 
		"GaussianBlurV_VS.cso", "GaussianBloom_PS.cso");
	horizontalBloomPostProcess.Initialize(screenQuadMeshPtr, 
		"GaussianBlurH_VS.cso", "GaussianBloom_PS.cso");
	combinePostProcess.Initialize(screenQuadMeshPtr, 
		"2DPassThrough_VS.cso", "PostCombineAdd_PS.cso");
	fogPostProcess.Initialize(screenQuadMeshPtr, 
		"2DPassThrough_VS.cso", "Fog_PS.cso");
	postTestProcess.Initialize(screenQuadMeshPtr, 
		"2DPassThrough_VS.cso", "PostTest_PS.cso");
	fxaaPostProcess.Initialize(screenQuadMeshPtr, 
		"2DPassThrough_VS.cso", "FXAA_PS.cso");

	// Initialize post process constant buffer values
	postShaderData.gTexCoordScale = 1;
	postShaderData.gFocusDistance = 15.0f;
	postShaderData.gFocusRange = 20.0f;
	postShaderData.gEdgeWidth = 1.0f;
	postShaderData.gEdgeIntensity = 5.0f;

    D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(cbPostProcess);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
	InitData.pSysMem = &postShaderData;

	HR(Renderer::theDevicePtr->CreateBuffer(&bd, &InitData, &postShaderDataCBuffer));

	Renderer::theContextPtr->VSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);
	Renderer::theContextPtr->PSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);
	Renderer::theContextPtr->GSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);

	// Check what post process effects should be enabled from settings file
	bool enable;
	EDUtilities::Settings::GetInstance()->GetSetting("EnableFog", enable, false);
	if(enable)
		AddFogProcess();
	EDUtilities::Settings::GetInstance()->GetSetting("EnableDepthOfField", enable, false);
	if(enable)
		AddDepthOfFieldPostProcess();
	EDUtilities::Settings::GetInstance()->GetSetting("EnableFXAA", enable, false);
	if(enable)
		AddFXAAPostProcess();

}

void PostProcessController::Clear(void)
{
	std::list< PostProcessChain * >::iterator processIter = 
		postProcessChains.begin();
	for( ; processIter != postProcessChains.end(); ++processIter)
	{	
		(*processIter)->Clear();
		delete (*processIter);
	}
	postProcessChains.clear();
}

void PostProcessController::UpdateResolution(void)
{
	list< PostProcessChain * >::iterator processIter = 
		postProcessChains.begin();
	for( ; processIter != postProcessChains.end(); ++processIter)
		(*processIter)->Initialize();
}

void PostProcessController::Process(EDRendererD3D::RenderTarget &renderTarget)
{
	list< PostProcessChain * >::iterator processIter = 
		postProcessChains.begin();
	for( ; processIter != postProcessChains.end(); ++processIter)
		(*processIter)->Run(renderTarget);
}

void PostProcessController::PushBackPostProcess(EDRendererD3D::PostProcess *process, 
										   PostProcessChain::PostProcessTransistion _transition)
{
	if(postProcessChains.empty())
	{
		PostProcessChain *newChain = new PostProcessChain(postShaderData);
		newChain->Initialize();
		postProcessChains.push_back(newChain);
	}
	PostProcessChain *lastChain = postProcessChains.back();
	lastChain->AddPostProcess(process, _transition);
}

void PostProcessController::RemovePostProcess(EDRendererD3D::PostProcess *process)
{
	auto currentProcessChain = postProcessChains.begin();
	auto endProcessChain = postProcessChains.end();

	for(; currentProcessChain != endProcessChain; currentProcessChain++)
	{
		if(0 == (*currentProcessChain)->RemovePostProcess(process))
		{
			// This chain is now empty and should be removed
			delete (*currentProcessChain);
			postProcessChains.erase(currentProcessChain);
			break;
		}
	}
}

void PostProcessController::UpdatePostProcessCBuffer(void)
{
	Renderer::theContextPtr->VSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);
	Renderer::theContextPtr->PSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);
	Renderer::theContextPtr->GSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);
}
