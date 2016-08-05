#pragma once

//#include "../EDCollision/CollidableTypes.h"
#include "ExampleGame.h"

/// The main game class.
class GameBase
{
protected:
	/// used to track frames per second.  This is kept as a member as it is not computed each frame.
	float fps;

	//EDCollision::FrustumHull frustum;

	ExampleGame exampleGame;
public:
	GameBase(void);
	/// Deletes test objects and singleton instances that should no longer be used.
	virtual ~GameBase(void);

	/// Method for setting the camera projection and frustum shape
	virtual void Resize(int width, int height);
	/// Initializes the main game data.
	virtual void Initialize();
	/// Updates main game objects.  Currently adds RenderShapes to batches for rendering.  
	/// This will be done by the spatial system in the near future.
	virtual void Update();
	/// Renders the RenderController, PrintConsole and InputConsole and swaps buffers.
	virtual void Render() = 0;

	ExampleGame &GetGameCore() { return exampleGame; }
};

