#include <windows.h>
#include "ExampleGame.h"
#include "../EDGameCore/Camera.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/RigidBody.h"
#include "../EDRendererD3D/ViewPortManager.h"
#include "RenderController.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDRendererD3D/GraphicsProfiler.h"
#include "ShapeRenderer.h"
#include "KeyboardController.h"
#include "MouseLookController.h"
#include "GamepadController.h"

#include "Mover.h"
#include "BuggyDriver.h"
#include "PointLightSource.h"
#include "SpotLightSource.h"
#include "DirectionalLightSource.h"
#include "FollowObserver.h"
#include "../EDUtilities/ContentManager.h"
#include "RenderController.h"
#include "SkyBox.h"
#include "JumpPad.h"
#include "Manipulator.h"
#include "PlayerController.h"
#include "Seeker.h"
#include "Evader.h"
#include "LookAt.h"
#include "TurnTo.h"
#include "PythonBehavior.h"
#include "PIDFollower.h"
#include "Picking.h"
#include "Target.h"
#include "HardAttach.h"
#include "Crosshair.h"
#include "Blast.h"
#include "ExplodeOnCollide.h"
#include "Sparkle.h"
#include "TerrainController.h"
#include "../EDUtilities/Settings.h"

#include "../EDRendererD3D/RenderShapeSkinned.h"
#include "../EDGameCore/Animation.h"

void ExampleGame::Initialize(void)
{
	Game::Initialize();

	string which_level;
	Settings::GetInstance()->GetSetting("Level", which_level, "Levels/empty_level.xml");
	//LoadScene(which_level.c_str());
	LoadGame(which_level.c_str());
}

void ExampleGame::RegisterStrings(void)
{
	//RegisterString("MainBuggy");
	//RegisterString("MainBuggyGun");
	//RegisterString("MainCamera");
	//RegisterString("Box");
}

void ExampleGame::RegisterTypes(void)
{
	RegisterType(ShapeRenderer);
	RegisterType(Mover);
	RegisterType(BuggyDriver);
	RegisterType(KeyboardController);
	RegisterType(MouseLookController);
	RegisterType(GamepadController);
	RegisterType(PointLightSource);
	RegisterType(SpotLightSource);
	RegisterType(DirectionalLightSource);
	//RegisterType(FollowObserver);
	RegisterType(SkyBox);
	RegisterType(JumpPad);
	RegisterType(Manipulator);
	RegisterType(PlayerController);
	RegisterType(Seeker);
	RegisterType(Evader);
	RegisterType(LookAt);
	RegisterType(TurnTo);
	RegisterType(PIDFollower);
	RegisterType(PythonBehavior);
	RegisterType(Picking);
	RegisterType(Target);
	RegisterType(HardAttach);
	RegisterType(Crosshair);
	RegisterType(Blast);
	RegisterType(ExplodeOnCollide);
	RegisterType(Sparkle);
	RegisterType(TerrainController);
}
// #1
void ExampleGame::ProcessCamera(void)
{
	EDGameCore::Camera* camera = GetCurrentCamera();


	EDGameCore::Camera::CameraRect rect = camera->GetCameraRect();
	float diffX = rect.maxX - rect.minX;
	float diffY = rect.maxY - rect.minY;

	float ratio = (diffX * GetWindowWidth()) / (diffY * GetWindowHeight());
	camera->SetAspectRatio(ratio);

	EDRendererD3D::ViewPortManager::GetReference().UpdateViewPort(
		camera->GetDepth(), camera->GetGameObject()->GetTransform()->GetWorldMatrix(), camera->GetViewMatrix(), camera->GetProjectionMatrix(), 
		camera->GetNearDistance(), camera->GetFarDistance());
}

// HACK light yo
EDRendererD3D::DirectionalLightWithShadow *hackLightPtr;

// #4 
// This method clears the current (camera) view set.
// Creates a view set of objects based on the light volume.
// Renders the given light, processing only the objects in the lights view for shadows
void ExampleGame::ProcessLight(EDGameCore::ILight* light)
{
	RenderController::GetInstance()->ClearLightRenderSets();

	light->Render();

	ForwardLightManager &forwardLightManager = RenderController::GetInstance()->GetForardLightManager();
	// If this is a shadow casting light, rebuild the view set for the given light
	if( EDGameCore::ILight::NONE != light->GetShadows())
	{
		BuildLightViewSet(light); 
	}

	switch(light->GetLightType())
	{
	case EDGameCore::ILight::LightType::POINT:
		{
			EDRendererD3D::GraphicsProfiler::GetReference().BeginEvent(0, 
				L"Rendering a point light");

			RenderController::GetInstance()->RenderPointLight((PointLightSource *)light);
			EDRendererD3D::GraphicsProfiler::GetReference().EndEvent();
			forwardLightManager.AddPointLight(((PointLightSource *)light)->GetLightPtr());
		}
		break;
	case EDGameCore::ILight::LightType::SPOT:
		{
			EDRendererD3D::GraphicsProfiler::GetReference().BeginEvent(0, 
				L"Rendering a spot light");

			RenderController::GetInstance()->RenderSpotLight((SpotLightSource *)light);
			EDRendererD3D::GraphicsProfiler::GetReference().EndEvent();
			forwardLightManager.AddSpotLight(((SpotLightSource *)light)->GetLightPtr());
		}
		break;
	case EDGameCore::ILight::LightType::DIRECTIONAL:
		{
			EDRendererD3D::GraphicsProfiler::GetReference().BeginEvent(0, 
				L"Rendering a directional light");

			hackLightPtr = ((DirectionalLightSource *)light)->GetLightPtr();

			RenderController::GetInstance()->RenderDirectionalLight((DirectionalLightSource *)light,
				GetCurrentCamera());

			EDRendererD3D::GraphicsProfiler::GetReference().EndEvent();
			forwardLightManager.AddDirectionalLight(((DirectionalLightSource *)light)->GetLightPtr());
		}
		break;
	default:
		break;
	};
}
// #6
// This method is currently being used to rebuild the camera view set after the lighting stage.
// The camera's view set is required again to perform forward pass rendering operations
void ExampleGame::ProcessRendererPostLit(EDGameCore::IRenderer* renderer)
{
	renderer->Render();
}

// #2
// This method builds the main camera view set
void ExampleGame::ProcessRendererPreLit(EDGameCore::IRenderer* renderer)
{
	renderer->Render();
}
// #8
// This method is in charge of handling any final pass rendering and clean up needed.
void ExampleGame::PostProcessCamera(void)
{
	hackLightPtr->ApplyCBuffer();
	RenderController::GetInstance()->PostRender();
}
// #5
// This method clears the current view set, and adds any objects not handled by the spatial system
// to the view set before entering the ProcessRendererPostLit stage
void ExampleGame::ProcessVisibleLightSet(void)
{
	RenderController::GetInstance()->RenderDebugPrimitives();

	RenderController::GetInstance()->ClearRenderSets();
	if( Game::GetSkyBox() != 0 )
		Game::GetSkyBox()->Render();
}

// #3 
// This method handles rendering of geometry data
void ExampleGame::ProcessPreLitVisibleSet(void)
{
	EDRendererD3D::GraphicsProfiler::GetReference().BeginEvent(0, L"Render GBuffers Begin!");
	RenderController::GetInstance()->RenderGBuffers();
	EDRendererD3D::GraphicsProfiler::GetReference().EndEvent();
}

// #7 
// This method executes rendering forward rendered objects
void ExampleGame::ProcessPostLitVisibleSet(void)
{
	RenderController::GetInstance()->GetForardLightManager().ApplyForwardLights();
	RenderController::GetInstance()->RenderUnlitObjects();
	RenderController::GetInstance()->RenderTransparentObjects();
}

void ExampleGame::BuildLightViewSet(EDGameCore::ILight *light)
{
	auto func = [&](EDGameCore::IRenderer *renderPtr)
	{
		ShapeRenderer *shapeRenderer = static_cast<ShapeRenderer *>(renderPtr);
		shapeRenderer->Render(nullptr);
	};
	light->ForEachRendererInLight(func);
}
