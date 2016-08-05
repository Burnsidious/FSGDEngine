#include "RenderController.h"
#include "WorldData.h"
#include "InputConsole.h"
#include "PostProcessController.h"


//#define VSG_DEFAULT_RUN_FILENAME "C:/Users/Burnside/Documents/Projects/fsgdengine/post.vsglog"
//#include <vsgcapture.h>

#include "../EDEventManager/EventManager.h"

#include "../EDTerrain/TerrainManager.h"
using namespace EDTerrain;
using EDRendererD3D::PostProcess;
using EDRendererD3D::RenderContext;
using std::iterator;

float GetFloatFromString(const void *stringData)
{
	wchar_t * strVal = (wchar_t*)stringData;

	return (float)wcstod(strVal, 0);
}

void SetDepthOffsetCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetShadowDepthOffset(val);
}

void SetDisplacementScaleCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetDisplacementScale(val);
}

void SetFocusDistanceCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	PostProcessController::GetInstance()->SetFocusDistance(val);
}

void SetFocusRangeCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	PostProcessController::GetInstance()->SetFocusRange(val);
}

void SetEdgeIntensityCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	PostProcessController::GetInstance()->SetEdgeIntensity(val);
}

void SetEdgeWidthCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	PostProcessController::GetInstance()->SetEdgeWidth(val);
}

void SetShadowDepthOffsetCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetShadowDepthOffset(val);
}

void AddDoFCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddDepthOfFieldPostProcess();
}

void AddDrawEdgesCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddDrawEdgesPostProcess();
}

void AddSobelEdgesCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddSobelEdgesPostProcess();
}

void AddSobelNormEdgesCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddSobelNormEdgesPostProcess();
}

void AddHorizontalBlurCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddGaussianBlurHPostProcess();
}

void AddVerticalBlurCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddGaussianBlurVPostProcess();
}

void ClearPostCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->Clear();
}

void AddDownSampleCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddDownSamplePostProcess();
}

void AddUpSampleCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddUpSamplePostProcess();
}

void AddHorizontalBloomCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddBloomHPostProcess();
}

void AddVerticalBloomCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddBloomVPostProcess();
}

void AddBrightPassCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddBrightPassPostProcess();
}

void AddCombineCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddCombinePostProcess();
}

void AddBloomCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddBloomPostProcess();
}

void AddFXAACallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddFXAAPostProcess();
}

void AddPostChainCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddChainPostProcess();
}

void AddPostTestCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddPostTestProcess();
}

void ReloadPostCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	RenderController::GetInstance()->CreatePostProcessEffects();
}

void AddFogCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostProcessController::GetInstance()->AddFogProcess();
}

void ToggleTerrainWireframe(unsigned int eventId, const void *eventData, void *listener)
{
	TerrainManager &tManger = TerrainManager::GetReference();

	tManger.SetWireframe(!tManger.GetWireframe());
}

void SetTessTriWidthCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	TerrainManager::GetReference().SetTessTriWidth(val);
}

void SetTerrainHeightScaleCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	TerrainManager::GetReference().SetHeightMapScale(val);
}

void SetTerrainHeightOffsetCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	TerrainManager::GetReference().SetHeightMapOffset(val);
}

void SetHeightTexCoordScaleCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	TerrainManager::GetReference().SetHeightTexCoordScale(val);
}

void SetDiffuseTexCoordScaleCallback(unsigned int eventId, const void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	TerrainManager::GetReference().SetDiffuseTexCoordScale(val);
}

void ToggleShowTilesCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	TerrainManager::GetReference().ToggleShowTiles();
}

void ToggleShowTessTransisitionsCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	TerrainManager::GetReference().ToggleShowTessTransisitions();
}

void ToggleShowTessTestCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	TerrainManager::GetReference().ToggleTerrainTest();
}

void RenderController::InitializeConsoleCommands()
{
	InputConsole &console = InputConsole::GetReference();
	// Register for events
	console.RegisterCommand(L"AddPostTest", 0, AddPostTestCallBack);
	console.RegisterCommand(L"ReloadPost", 0, ReloadPostCallBack);
							
	console.RegisterCommand(L"SetDepthOffset", 0, SetDepthOffsetCallBack);
	console.RegisterCommand(L"SetEdgeIntensity", 0, SetEdgeIntensityCallBack);
	console.RegisterCommand(L"SetEdgeWidth", 0, SetEdgeWidthCallBack);
	console.RegisterCommand(L"SetShadowDepthOffset", 0, SetShadowDepthOffsetCallBack);
	console.RegisterCommand(L"SetDisplacementScale", 0, SetDisplacementScaleCallBack);
							
	console.RegisterCommand(L"AddDoF", 0, AddDoFCallBack);
	console.RegisterCommand(L"AddFog", 0, AddFogCallBack);
	console.RegisterCommand(L"AddEdges", 0, AddDrawEdgesCallBack);
	console.RegisterCommand(L"AddSobelEdges", 0, AddSobelEdgesCallBack);
	console.RegisterCommand(L"AddSobelNormaEdges", 0, AddSobelNormEdgesCallBack);
	
	console.RegisterCommand(L"AddBlurH", 0, AddHorizontalBlurCallBack);
	console.RegisterCommand(L"AddBlurV", 0, AddVerticalBlurCallBack);
							
	console.RegisterCommand(L"AddDown", 0, AddDownSampleCallBack);
	console.RegisterCommand(L"AddUp", 0, AddUpSampleCallBack);
	console.RegisterCommand(L"AddBloomH", 0, AddHorizontalBloomCallBack);
	console.RegisterCommand(L"AddBloomV", 0, AddVerticalBloomCallBack);
	console.RegisterCommand(L"AddBrightPass", 0, AddBrightPassCallBack);
	console.RegisterCommand(L"AddCombine", 0, AddCombineCallBack);
	console.RegisterCommand(L"AddBloom", 0, AddBloomCallBack);
							
	console.RegisterCommand(L"ClearPost", 0, ClearPostCallBack);
	console.RegisterCommand(L"SetFocusDistance", 0, SetFocusDistanceCallBack);
	console.RegisterCommand(L"SetFocusRange", 0, SetFocusRangeCallBack);
							
	console.RegisterCommand(L"FXAA", 0, AddFXAACallBack);
							
	console.RegisterCommand(L"AddPostChain", 0, AddPostChainCallBack);
							
	console.RegisterCommand(L"ToggleTerrainWireframe", 0, ToggleTerrainWireframe);
	console.RegisterCommand(L"SetTessTriWidth", 0, SetTessTriWidthCallBack);
	console.RegisterCommand(L"SetTerrainHeightScale", 0, SetTerrainHeightScaleCallBack);
	console.RegisterCommand(L"ToggleShowTiles", 0, ToggleShowTilesCallBack);
	console.RegisterCommand(L"SetTerrainHeightTexCoordScale", 0, SetHeightTexCoordScaleCallBack);
	console.RegisterCommand(L"SetTerrainDiffuseTexCoordScale", 0, SetDiffuseTexCoordScaleCallback);
	console.RegisterCommand(L"SetTerrainHeightOffset", 0, SetTerrainHeightOffsetCallBack);
	console.RegisterCommand(L"ToggleShowTessTransisitions", 0, ToggleShowTessTransisitionsCallBack);
	console.RegisterCommand(L"TTT", 0, ToggleShowTessTestCallBack);
}

void RenderController::SetShadowDepthOffset(float val)
{
	miscBuffer.gShadowDepthOffset = val;
	UpdateMiscShaderData();
}

void RenderController::SetDisplacementScale(float val)
{
	miscBuffer.gDisplacementScale = val;
	UpdateMiscShaderData();
}

void PostProcessController::SetEdgeWidth(float val)
{
	postShaderData.gEdgeWidth = val;
}

void PostProcessController::SetEdgeIntensity(float val)
{
	postShaderData.gEdgeIntensity = val;
}

void PostProcessController::SetFocusDistance(float val)
{
	postShaderData.gFocusDistance = val;
}

void PostProcessController::SetFocusRange(float val)
{
	postShaderData.gFocusRange = val;
}

void PostProcessController::AddDepthOfFieldPostProcess()
{
	PostProcessChain *newChain = new PostProcessChain(postShaderData);
	newChain->Initialize();
	postProcessChains.push_back(newChain);

	PushBackPostProcess(&horizontalBlurPostProcess, PostProcessChain::PPT_DownSample);
	PushBackPostProcess(&verticalBlurPostProcess, PostProcessChain::PPT_DownSample);
	//PushBackPostProcess(&horizontalBlurPostProcess);
	//PushBackPostProcess(&verticalBlurPostProcess);
	//PushBackPostProcess(&horizontalBlurPostProcess);
	//PushBackPostProcess(&verticalBlurPostProcess);
	//PushBackPostProcess(&horizontalBlurPostProcess);
	//PushBackPostProcess(&verticalBlurPostProcess);
	//PushBackPostProcess(&horizontalBlurPostProcess);
	//PushBackPostProcess(&verticalBlurPostProcess);
	PushBackPostProcess(&horizontalBlurPostProcess, PostProcessChain::PPT_UpSample);
	PushBackPostProcess(&verticalBlurPostProcess, PostProcessChain::PPT_UpSample);
	PushBackPostProcess(&depthOfFieldPostProcess);

}

void PostProcessController::AddDrawEdgesPostProcess()
{
	PushBackPostProcess(&edgePostProcess);
}

void PostProcessController::AddSobelEdgesPostProcess()
{
	PushBackPostProcess(&edgeSobelPostProcess);
}

void PostProcessController::AddSobelNormEdgesPostProcess()
{
	PushBackPostProcess(&edgeSobelNormPostProcess);
}

void PostProcessController::AddGaussianBlurHPostProcess()
{
	PushBackPostProcess(&horizontalBlurPostProcess);
}

void PostProcessController::AddGaussianBlurVPostProcess()
{
	PushBackPostProcess(&verticalBlurPostProcess);
}

void PostProcessController::AddDownSamplePostProcess()
{
	PushBackPostProcess(&passThroughPostProcess, PostProcessChain::PPT_DownSample);
}

void PostProcessController::AddUpSamplePostProcess()
{
	PushBackPostProcess(&passThroughPostProcess, PostProcessChain::PPT_DownSample);
}

void PostProcessController::AddBloomHPostProcess()
{
	PushBackPostProcess(&horizontalBloomPostProcess);
}

void PostProcessController::AddBloomVPostProcess()
{
	PushBackPostProcess(&verticalBloomPostProcess);
}

void PostProcessController::AddCombinePostProcess()
{
	PushBackPostProcess(&combinePostProcess);
}

void PostProcessController::AddBrightPassPostProcess()
{
	PushBackPostProcess(&brightPostProcess);
}

void PostProcessController::AddChainPostProcess()
{
	PostProcessChain *newChain = new PostProcessChain(postShaderData);
	newChain->Initialize();
	postProcessChains.push_back(newChain);
}

void PostProcessController::AddBloomPostProcess()
{
	PostProcessChain *newChain = new PostProcessChain(postShaderData);
	newChain->Initialize();
	postProcessChains.push_back(newChain);

	PushBackPostProcess(&brightPostProcess, PostProcessChain::PPT_DownSample);
	PushBackPostProcess(&horizontalBloomPostProcess, PostProcessChain::PPT_DownSample);
	PushBackPostProcess(&verticalBloomPostProcess);
	PushBackPostProcess(&horizontalBloomPostProcess, PostProcessChain::PPT_UpSample);
	PushBackPostProcess(&verticalBloomPostProcess, PostProcessChain::PPT_UpSample);

	PushBackPostProcess(&combinePostProcess);
}

void PostProcessController::AddFXAAPostProcess()
{
	PushBackPostProcess(&fxaaPostProcess);
}

void PostProcessController::AddPostTestProcess()
{
	PushBackPostProcess(&postTestProcess);
}

void PostProcessController::AddFogProcess()
{
	PushBackPostProcess(&fogPostProcess);
}

