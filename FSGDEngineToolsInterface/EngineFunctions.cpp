#include "EngineFunctions.h"
#include "ToolRenderController.h"

#include "../EDUtilities/ContentManager.h"

#include "../EDRendererD3D/RenderForm.h"
#include "../EDRendererD3D/RenderShape.h"

using namespace FSGDEngineToolInterface;
using namespace EDUtilities;
using namespace EDRendererD3D;

ContentHandle<RenderForm> testFormHandle;
RenderShape * shapeTestPtr = nullptr;

void InitializeRenderer(HWND windowHandle)
{
	ContentManager::theContentPath = "../FSGDGame/Resources/GDAssets/";

	ToolRenderController::GetReference().Initialize(windowHandle);

	//mainObject = mainScene.SpawnObject("GameObjects/GO_Buggy.xml");
}

void InitializeScene(void)
{
	//testFormHandle = ContentManager::LoadXML<RenderForm>(
	//	"GDForm/VZ_Robot_Walk/robotkid/robotkidShape/frm_mat_blinn1_rc_LitTexturedShadowsNotSkinned.xml");

	//shapeTestPtr = (RenderShape *)testFormHandle.GetContent()->GetNewRenderShape();

}

void RenderMainView(void)
{
	ToolRenderController::GetReference().Render();
}

void ShutdownRenderer(void)
{
	delete shapeTestPtr;
	testFormHandle.Release();
	ToolRenderController::GetReference().Shutdown();
	ContentManager::DeleteInstance();

}