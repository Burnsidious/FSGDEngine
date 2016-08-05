#pragma once

#include "GameBase.h"
#include "PauseEffect.h"
#include "FSGDOcclusionQuery.h"

//#include "../EDGOCA/GameObject.h"
//#include "../EDGOCA/Scene.h"

/// The main game class.
class Game : public GameBase
{
private:

	//EDGOCA::Scene testScene;

	//std::vector<EDGOCA::GameObject *> cameras;

	//EDGOCA::GameObject *lightObject;

	/// pause state boolean
	bool isPaused;

	/// An effect to be run while paused
	PauseEffect pauseEffect;

	EDRendererD3D::RenderSprite crossHairSprite;

	/// GUI element to show while paused
	EDRendererD3D::RenderSprite pauseSprite;

	/// Sprite to represent background of the print console
	EDRendererD3D::RenderSprite consoleBackground;

	/// Initializes test entities and their components.
	void InitTestObjects();

	/// Method to setup hard coded eventys and input handling
	void InitializeEvents(void);
public:
	Game(void);
	/// Deletes test objects and singleton instances that should no longer be used.
	~Game(void);

	/// Method for setting the camera projection and frustum shape
	void Resize(int width, int height);
	/// Initializes the main game data.
	void Initialize();
	/// Updates main game objects.  Currently adds RenderShapes to batches for rendering.  
	/// This will be done by the spatial system in the near future.
	void Update();
	/// Renders the RenderController, PrintConsole and InputConsole and swaps buffers.
	void Render();

	void TogglePause();

	/// Method used to switch controls from one entity to another
	void ToggleActiveController();

	void ToggleActiveViewer();

	void ExitSplashScreen();

	void SetOcclusionCullingState(bool state);

	static void InstantiateCallback(unsigned int eventId, const void *eventData, void *listener);
};