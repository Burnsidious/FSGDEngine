#pragma once
#include "../EDUtilities/Timer.h"

class Game;

class WorldData
{
    friend class GameBase;
	friend class Game;
	friend class ArchGame;

private:
    /// Timer object used to track world state times
    static EDUtilities::Timer timer;
    /// The time between frames
    static float theDeltaTime;
    /// The width of the screen in pixels.
	static UINT theScreenWidth;
    /// The height of the screen in pixels.  This value includes the file menu.
	static UINT theScreenHeight;

    static void UpdateDeltaTime()
    {
        theDeltaTime =  timer.GetElapsedTime();
	    timer.Reset();
    }
public:
    /// \return Returns the amount of time passed between frames.
    inline static float GetDeltaTime() { return theDeltaTime; }
    /// \return Returns the width of the screen in pixels.
	inline static UINT GetScreenWidth() { return theScreenWidth; }
    /// \return Returns the height of the screen in pixels.  This includes the file menu.
	inline static UINT GetScreenHeight() { return theScreenHeight; }

	static void SetScreenWidth(UINT screenWidth) { theScreenWidth = screenWidth; }
	static void SetScreenHeight(UINT screenHeight) { theScreenHeight = screenHeight; }

};
