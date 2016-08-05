#include "GameBase.h"


#include "../EDUtilities/Timer.h"
#include "../EDMemoryManager/MemoryManager.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/IndexBuffer.h"
#include "../EDRendererD3D/ViewPortManager.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDEventManager/EventManager.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDRendererD3D/TextRenderer.h"
#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/Settings.h"
#include "../EDUtilities/PythonModule.h"

#include "WorldData.h"
#include "InputConsole.h"
#include "RenderController.h"

//using namespace EDCollision;
using namespace EDRendererD3D;
using namespace EDUtilities;
using namespace EDMath;

extern HWND globalHWnd;
 
GameBase::GameBase(void) 
{
	Settings::GetInstance()->LoadFromXML("Resources/GDAssets/Settings/settings.xml");
	PythonModule::Initialize();

	int width = 0, height = 0;
	EDUtilities::Settings::GetInstance()->GetSetting("Width", width, 1280);
	EDUtilities::Settings::GetInstance()->GetSetting("Height", height, 720);
	WorldData::SetScreenWidth(width);
	WorldData::SetScreenHeight(height);
	
	fps = 0.0f;

	//WorldData::SetScreenHeight(900);
	//WorldData::SetScreenWidth(1600);
}

GameBase::~GameBase(void)
{
	exampleGame.Shutdown();
	PythonModule::Shutdown();
	TextRenderer::DeleteInstance();
	DebugRenderer::DeleteInstance();
	InternalOutput::DeleteInstance();
	VertexBufferManager::DeleteInstance();
	IndexBuffer::DeleteInstance();
	InputConsole::DeleteInstance();
	PrintConsole::DeleteInstance();
	ViewPortManager::DeleteInstance();
	RenderController::DeleteInstance();
	ContentManager::DeleteInstance();
	Renderer::Shutdown();
}

void ToggleMemMetrics(unsigned int _eventId, const void* _eventData, void* _listener)
{
	EDMemoryManager::MemoryManager& mem = (*EDMemoryManager::MemoryManager::GetInstance());

	mem.SetMetricState( (mem.GetMetricState()) ? false : true);
}

void GameBase::Initialize() 
{   
	WorldData::timer.Reset();

	Renderer::Initialize(globalHWnd, WorldData::GetScreenWidth(), WorldData::GetScreenHeight());
	PrintConsole::GetReference().Initialize();

	RenderController::GetInstance()->Initialize();
	//PrintConsole::GetReference().CreateRenderFont(Renderer::theDevicePtr);

	InputConsole::GetReference().RegisterCommand(L"ToggleMemMetrics", 0, ToggleMemMetrics);
}

void GameBase::Resize(int width, int height)
{
	// update the directional light projection matrix
	//RenderController::GetInstance()->BuildInitialiDirMatrices(frustum);

	Renderer::ResizeBuffers();

	RenderController::GetInstance()->UpdateResolution();
}

void GameBase::Update()
{
	const int FPS_UPDATE_RATE = 5;

	// Track frame rate
	static int numFrames = 0;
	static Timer frameTimer;
	++numFrames;

	if(FPS_UPDATE_RATE < numFrames)
	{
		fps = FPS_UPDATE_RATE / frameTimer.GetElapsedTime();
		numFrames = 0;
		frameTimer.Reset();
	}

	//Timer
	WorldData::UpdateDeltaTime();
	float fDeltaTime = WorldData::GetDeltaTime();

	PrintConsole::GetReference().AddString(PrintConsole::PPN_Render, Float3(1.0f, 0.0f, 0.0f), 
		L"FPS = %f", fps);

	// Output console Controls page text
	PrintConsole::GetReference().AddString(PrintConsole::PPN_Controls, Float3(1.0f, 0.0f, 0.0f), L"o = toggle this output console");
	PrintConsole::GetReference().AddString(PrintConsole::PPN_Controls, Float3(1.0f, 0.0f, 0.0f), L"i = next page in output console");
	PrintConsole::GetReference().AddString(PrintConsole::PPN_Controls, Float3(1.0f, 0.0f, 0.0f), L"u = previous page in output console");
	PrintConsole::GetReference().AddString(PrintConsole::PPN_Controls, Float3(1.0f, 0.0f, 0.0f), L"k = next class on solutions page");
	PrintConsole::GetReference().AddString(PrintConsole::PPN_Controls, Float3(1.0f, 0.0f, 0.0f), L"l = next lab on solutions page");
	PrintConsole::GetReference().AddString(PrintConsole::PPN_Controls, Float3(1.0f, 0.0f, 0.0f), L"w,a,s,d = translate camera in local space");
	PrintConsole::GetReference().AddString(PrintConsole::PPN_Controls, Float3(1.0f, 0.0f, 0.0f), L"right mouse button = rotate camera based on mouse movement");
	PrintConsole::GetReference().AddString(PrintConsole::PPN_Controls, Float3(1.0f, 0.0f, 0.0f), L"i,j,k,l,n,m = translate test point light");

	EDMemoryManager::MemoryManager& mem = (*EDMemoryManager::MemoryManager::GetInstance());

	if (mem.GetMetricState())
	{
		size_t totalMemoryAvailable = mem.GetTotalMemory();
		size_t currentMemoryUsage = mem.GetTotalMemoryUsed();
		float memoryUsagePercentage = (currentMemoryUsage / (float)totalMemoryAvailable) * 100.0f;

		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"Heap Size = %d", totalMemoryAvailable);
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"Memory Used = %d", currentMemoryUsage);
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"Memory Free = %d", mem.GetTotalMemoryAvailable());
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"%% Usage = %f", memoryUsagePercentage);
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"Descriptor Size = %d", mem.GetDescriptorSize());

		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"Allocations = %d", mem.GetNumberOfAllocations());
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"Smallest = %d", mem.GetSmallestAllocation());
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"Largest = %d", mem.GetLargestAllocation());
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"Current Average = %d", mem.GetAverageAllocation());
	}
	else
	{
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"Memory metrics disabled (default for release mode)");
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"To enable, use command ToggleMemMetrics from input console (~)");
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, Float3(1.0f, 0.0f, 0.0f), L"Metrics and leak detection will be innacurate if data collection is ever disabled");
	}

	// hack-ish, had issues double iterating within a set, also the characters are not mono-spaced so I could not simulate a tab
	const std::set<const std::wstring>& commands = InputConsole::GetReference().GetCommands();
	bool col_1 = true;
	wstring to_add;
	for(auto iter = commands.begin(); iter != commands.end(); ++iter)
	{
		to_add.append((*iter).c_str());
		to_add.append(L"       ");

		if (col_1 == false)
		{
			PrintConsole::GetReference().AddString(PrintConsole::PPN_COMMANDS, Float3(1.0f, 0.0f, 0.0f), to_add.c_str());
			to_add.clear();
		}

		col_1 = !col_1;
	}

	EDEventManager::EventManager::GetInstance()->Update();
}
