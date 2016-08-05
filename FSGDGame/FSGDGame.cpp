#include "FSGDGame.h"

#include "InputConsole.h"
#include "WorldData.h"
#include "Behaviors.h"

#include "../EDEventManager/EventManager.h"
#include "../EDInput/InputManager.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/ViewPortManager.h"
#include "../EDRendererD3D/IndexBuffer.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDThreadPool/ThreadPool.h"
#include "../EDThreadPool/WorkerExample.h"
#include "../EDUtilities/PakFile.h"
#include "../EDMemoryManager/MemoryManager.h"
#include "../EDUtilities/Settings.h"
#include "../EDUtilities/PythonModule.h"

using namespace EDCollision;
using namespace EDRendererD3D;
using namespace EDUtilities;
using namespace EDThreadPool;
using namespace EDMath;
using std::string;

extern HWND globalHWnd;
extern GameBase *globalGamePtr;

inline float RandomFloat(float min, float max)
{
	return (((rand()/(float)RAND_MAX)*((max)-(min)))+(min));
}

Game::Game(void) 
{
	isPaused = false;
	EDUtilities::PakFile::GetInstance();
	EDThreadPool::ThreadPool::GetInstance();
}

Game::~Game(void)
{
	InputLayoutManager::DeleteInstance();
	EDThreadPool::ThreadPool::GetInstance()->Shutdown();
}

void ExitCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	PostQuitMessage(0);
}

void TogglePauseCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	((Game *)globalGamePtr)->TogglePause();
}

void ToggleActiveControllerCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	((Game *)globalGamePtr)->ToggleActiveController();
}

void ToggleActiveViewerCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	((Game *)globalGamePtr)->ToggleActiveViewer();
}
void RenderSplash(unsigned int eventId, const void *eventData, void *listener)
{
	((Game*)globalGamePtr)->ExitSplashScreen();
}

void ToggleOcclusionCullingCallBack(unsigned int eventId, const void *eventData, void *listener)
{
	bool state = true;
	char* param = (char*)eventData;

	if( param != 0 )
	{
		if(strcmp(param, "0") == 0)
			state = false;
		else if(_stricmp(param, "false") == 0)
			state = false;
	}

	((Game*)globalGamePtr)->SetOcclusionCullingState(state);
}

// spawns a test task for the thread pool
void PoolTaskExample(unsigned int _eventId, const void* _eventData, void* _listener)
{
	if (_eventData != nullptr)
	{
		WorkerExample* worker = new WorkerExample(wcstol((wchar_t*)_eventData, '\0', 10));
		ThreadPool::GetInstance()->PostWork(worker);
		ThreadPool::GetInstance()->testObjects.push_back(worker);
	}
}

void ExtractAsset(unsigned int _eventId, const void* _eventData, void* _listener)
{
	char buffer[256];
	if (_eventData != nullptr)
	{
		wcstombs_s(nullptr, buffer, 256, (const wchar_t*)_eventData, 256);
		EDUtilities::PakFile::GetInstance()->ExtractAsset(buffer);
	}
}

void CoreDump(unsigned int _eventId, const void* _eventData, void* _listener)
{
	EDUtilities::InternalOutput::GetReference() << "Commencing core dump to log file, this may take a while.\n";
	EDMemoryManager::MemoryManager::GetInstance()->CoreDump();
}

void MemMetricDump(unsigned int _eventId, const void* _eventData, void* _listener)
{
	EDUtilities::InternalOutput::GetReference() << "Commencing mem metric dump to log file.\n";
	EDMemoryManager::MemoryManager::GetInstance()->MetricDump();
}

void LoadSceneFromLevel(unsigned int _eventId, const void* _eventData, void* _listener)
{
	char buffer[256];
	if (_eventData != nullptr)
	{
		wcstombs_s(nullptr, buffer, 256, (const wchar_t*)_eventData, 256);
		EDGameCore::Game::LoadScene(buffer);
	}
}

void Game::SetOcclusionCullingState(bool state)
{
	//if( state == true )
	//	testScene.EnableSceneFlags( Scene::SF_OCCLUSION_CULL );
	//else
	//	testScene.DisableSceneFlags( Scene::SF_OCCLUSION_CULL );
}

void Game::ToggleActiveViewer()
{
	//if( m_pActiveViewer == m_pCameraEntity )
	//	m_pActiveViewer = m_pRobotKid;
	//else
	//	m_pActiveViewer = m_pCameraEntity;
}
void Game::ExitSplashScreen()
{
	//if(m_pSplashScreen)
	//{
	//	m_pSplashScreen->RemoveComponent(m_pSplashScreen->GetComponent("RenderableComponent"));
	//	m_Level.GetEntityList().remove(m_pSplashScreen);
	//	delete m_pSplashScreen;
	//	m_pSplashScreen = 0;
	//}
}

void Game::TogglePause()
{
	isPaused = !isPaused;
	if(isPaused)
		pauseEffect.Activate();
	else
		pauseEffect.Deactivate();

	pauseSprite.SetShouldRender(isPaused);
}

void Game::ToggleActiveController()
{
	//m_pSphereController->SetEnabled(!m_pSphereController->IsEnabled());
	//m_pPlayerController->SetEnabled(!m_pSphereController->IsEnabled());
}

void Game::InitializeEvents(void)
{
	using namespace EDInput;
	
	AddInputCallback(KEYBOARD_TYPE, VK_RETURN,		INPUT_PRESSED, "Splash", RenderSplash);
	AddInputCallback(KEYBOARD_TYPE, VK_ESCAPE,		INPUT_PRESSED, "Exit", ExitCallBack, nullptr, MODE_ANY);
	AddInputCallback(KEYBOARD_TYPE, 'P',			INPUT_PRESSED, "PauseGame", TogglePauseCallBack);
	AddInputCallback(KEYBOARD_TYPE, 'Z',			INPUT_PRESSED, "ToggleActiveController", ToggleActiveControllerCallBack);
	AddInputCallback(KEYBOARD_TYPE, 'V',			INPUT_PRESSED, "ToggleActiveViewer", ToggleActiveViewerCallBack);

	//Deprecated?  protoype of callback function changed?
	AddInputCallback(KEYBOARD_TYPE, 'O',			INPUT_PRESSED, "TogglePrintConsole",	reinterpret_cast<EDEventManager::EM_CALLBACK>(PrintConsole::ToggleConsoleCallBack) );
	AddInputCallback(KEYBOARD_TYPE, 'I',			INPUT_PRESSED, "NextPageConsole",		reinterpret_cast<EDEventManager::EM_CALLBACK>(PrintConsole::NextPageCallBack) );
	AddInputCallback(KEYBOARD_TYPE, 'U',			INPUT_PRESSED, "PreviousPageConsole",	reinterpret_cast<EDEventManager::EM_CALLBACK>(PrintConsole::PreviousPageCallBack) );
	AddInputCallback(KEYBOARD_TYPE, 'K',			INPUT_PRESSED, "NextClassConsole",		reinterpret_cast<EDEventManager::EM_CALLBACK>(PrintConsole::NextClassCallBack) );
	AddInputCallback(KEYBOARD_TYPE, 'L',			INPUT_PRESSED, "NextLabConsole",		reinterpret_cast<EDEventManager::EM_CALLBACK>(PrintConsole::NextLabCallBack) );

	//Legacy code?  Could not find where this was hooked up to
	//// Sets page up/down to change camera behaviors 
	//InputManager::AddKeyTrigger(VK_PRIOR, INPUT_RELEASED, "CamBehaviorPrev");
	//InputManager::AddKeyTrigger(VK_NEXT, INPUT_RELEASED, "CamBehaviorNext");
	AddInputCallback(KEYBOARD_TYPE, VK_OEM_3, INPUT_PRESSED, "ToggleConsole", 
		reinterpret_cast<EDEventManager::EM_CALLBACK>(InputConsole::ToggleConsoleCallback), nullptr, MODE_ANY);
	AddInputCallback(KEYBOARD_TYPE, VK_RIGHT, INPUT_PRESSED, "CompleteConsoleString", 
		reinterpret_cast<EDEventManager::EM_CALLBACK>(InputConsole::CompleteConsoleStringCallback), nullptr, TYPED_INPUT);
	AddInputCallback(KEYBOARD_TYPE, VK_UP, INPUT_PRESSED, "RememberConsoleString", 
		reinterpret_cast<EDEventManager::EM_CALLBACK>(InputConsole::RememberConsoleStringCallback), nullptr, TYPED_INPUT);
}

void Game::Initialize(void) 
{
	PythonModule test("engine_script");
	ContentManager::theContentPath = "Resources/GDAssets/";

	bool use_pak = false;
	Settings::GetInstance()->GetSetting("UsePakTextures", use_pak, false);
	if (use_pak)
	{
		PakFile::GetInstance()->LoadDirectory("Resources/GDAssets/Textures/textures.pak");
	}

	GameBase::Initialize();
	exampleGame.Initialize();

	// Testing Inputlayout manager
	InputLayoutManager::GetReference();
	//testScene.SetOcclusionQueryCreator( FSGDOcclusionQuery::creator );

	InitTestObjects();

	InitializeEvents();

	RECT consoleRect;
	consoleRect.top = 0;
	consoleRect.bottom = Renderer::GetResolutionHeight() / 2;
	consoleRect.left = 0;
	consoleRect.right = Renderer::GetResolutionHeight();

	ContentHandle<RenderTexture2D> spriteTexture = 
		ContentManager::Load<RenderTexture2D>("Textures/ConsoleBackground.dds");
	consoleBackground.Initialize(spriteTexture.GetContent()->GetShaderResourceView(), consoleRect);

	pauseEffect.Initialize();

	InputConsole::GetReference().RegisterCommand(L"ToggleOcclusionCulling", 0, ToggleOcclusionCullingCallBack);

	InputConsole::GetReference().RegisterCommand(L"LoadScene", 0, LoadSceneFromLevel);
	InputConsole::GetReference().RegisterCommand(L"PoolTask", 0, PoolTaskExample);
	InputConsole::GetReference().RegisterCommand(L"ExtractAsset", 0, ExtractAsset);
	InputConsole::GetReference().RegisterCommand(L"CoreDump", 0, CoreDump);
	InputConsole::GetReference().RegisterCommand(L"MetricDump", 0, MemMetricDump);
	InputConsole::GetReference().RegisterCommand(L"Instantiate", 0, InstantiateCallback);
}

void Game::InstantiateCallback(unsigned int eventId, const void *eventData, void *listener)
{
	wchar_t* param = (wchar_t*)eventData;

	if( param == 0 )
		return;

	EDGameCore::GameObject* cameraObj = EDGameCore::GameObject::Find("Camera");

	if( cameraObj == 0 )
		return;
	
	// Convert from unicode to ascii.
	// Only valid for characters found in both.
	unsigned int length = (unsigned int)wcslen(param) + 1;
	char* cparam = new char[ length ];
	for(unsigned int i = 0; i < length; ++i)
		cparam[i] = char(param[i]);

	cparam[length-1] = 0;

	char* instanceName = 0;
	char* countStr = 0;
	char* next_token = 0;
	
	instanceName = strtok_s( cparam, ",", &next_token );
	countStr = strtok_s( 0, ",", &next_token );

	int countX = 1;
	int countY = 1;
	int countZ = 1;

	if( countStr != 0 )
	{
		countX = atoi(countStr);

		countStr = strtok_s( 0, ",", &next_token );

		if( countStr != 0 )
		{
			countY = atoi(countStr);

			countStr = strtok_s( 0, ",", &next_token );

			if( countStr != 0 )
			{
				countZ = atoi(countStr);
			}
		}
	}

	if( countX < 1 || countY < 1 || countZ < 1 )
	{
		delete [] cparam;
		InternalOutput::GetReference().Error( "\nInvalid number of GameObjects to instantiate.", eventData );
		return;
	}

	EDGameCore::RegistrationId prefabNameId = EDGameCore::Game::GetStringRegistrationId(instanceName);

	if( prefabNameId == 0 )
	{
		InternalOutput::GetReference().Error( "\nGameObject Prefab \"%s\" not found.", eventData );
		delete [] cparam;
		return;
	}

	int halfX = countX / 2;

	Float4x4 camMat = cameraObj->GetTransform()->GetWorldMatrix();

	Float4x4 xform = camMat;
	
	for(int x = 0; x < countX; ++x)
	{
		for(int y = 0; y < countY; ++y)
		{
			for(int z = 0; z < countZ; ++z)
			{
				xform.WAxis = camMat.WAxis + (camMat.ZAxis * (float)(z+2)) + (camMat.XAxis * (float)(x - halfX)) + (camMat.YAxis * (float)y);
				EDGameCore::GameObject* gameObj = EDGameCore::Game::Instantiate( prefabNameId, xform );

				if( gameObj == 0 )
				{
					InternalOutput::GetReference().Error( "\nGameObject Prefab \"%s\" not found.", eventData );
					delete [] cparam;
					return;
				}
			}
		}
	}

	delete [] cparam;
}

void Game::Resize(int width, int height)
{
	// Update the cameras with the new aspect ratio
	// This data is being sent as a message to a GOCA object, 
	// the data format is perspective width, height, near, far.
	//float projectionProperties[4] = { (float)width, (float)height, 0.1f, 100.0f };
	//frustum.Build( 0.78539816f, projectionProperties[0], projectionProperties[1],
		//projectionProperties[2], projectionProperties[3]);

	//size_t numCameras = cameras.size();
	//for(size_t i = 0; i < numCameras; ++i)
	//{
	//	cameras[i]->OnMessage(&MessageT<float *>(FSGDGameMessages::MSG_BUILD_PROJECTION, projectionProperties));
	//}

	GameBase::Resize(width, height);
	exampleGame.SetWindowHeight( height );
	exampleGame.SetWindowWidth( width );
}

void Game::Update()
{
	GameBase::Update();

	exampleGame.Update();

	WCHAR buffer[128];
	swprintf_s(buffer, L"FPS = %f", fps);
	SetWindowText(globalHWnd, buffer);

	DebugRenderer::GetInstance()->RenderDebugPrimitives();
}

void Game::Render()
{
	consoleBackground.SetShouldRender(PrintConsole::GetReference().IsEnabled());

	InputConsole::GetReference().Render();

	exampleGame.Render();

	Renderer::Present();
}

void Game::InitTestObjects()
{
	// Cross hair sprite
	ContentHandle<RenderTexture2D> spriteTexture = 
		ContentManager::Load<RenderTexture2D>("Textures/crosshair.dds");

	UINT screenCenterY = Renderer::GetResolutionHeight() / 2;
	UINT screenCenterX = Renderer::GetResolutionWidth() / 2;
	UINT halfSize = Renderer::GetResolutionWidth() / 60;

	RECT testRect;
	testRect.left = screenCenterX - halfSize;
	testRect.right = screenCenterX + halfSize;
	testRect.top = screenCenterY - halfSize;
	testRect.bottom = screenCenterY + halfSize;
	crossHairSprite.Initialize(spriteTexture.GetContent()->GetShaderResourceView(), testRect);
	crossHairSprite.SetShouldRender(false);

	spriteTexture = ContentManager::Load<RenderTexture2D>("Textures/pause.dds");

	pauseSprite.Initialize(spriteTexture.GetContent()->GetShaderResourceView(), testRect);
	pauseSprite.SetShouldRender(false);
}
