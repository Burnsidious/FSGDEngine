#include "precompiled.h"
#include "GraphicsProfiler.h"

namespace EDRendererD3D
{
    GraphicsProfiler *GraphicsProfiler::instancePtr = 0;

	GraphicsProfiler::GraphicsProfiler(void)
	{
		dx9DLL = 0;
	}


	GraphicsProfiler::~GraphicsProfiler(void)
	{
	}

    GraphicsProfiler &GraphicsProfiler::GetReference()
    {
        if(!instancePtr)
        {
			instancePtr = new GraphicsProfiler;
			if(!instancePtr->Initialize())
			{
				DeleteInstance();
			}
        }
        return *instancePtr;
    }

    void GraphicsProfiler::DeleteInstance()
    {
		delete instancePtr;
		instancePtr = nullptr;
    }

	bool GraphicsProfiler::Initialize()
	{
		dx9DLL = LoadLibrary(L"d3d9.dll");
		if(0 == dx9DLL)
			return false;

		BeginEvent = (BeginEventSigniture)GetProcAddress(dx9DLL, "D3DPERF_BeginEvent");
		EndEvent = (EndEventSigniture)GetProcAddress(dx9DLL, "D3DPERF_EndEvent");

		SetMarker = (SetMarkerSigniture)GetProcAddress(dx9DLL, "D3DPERF_SetMarker");
		SetRegion = (SetRegionSigniture)GetProcAddress(dx9DLL, "D3DPERF_SetRegion");
		QueryRepeatFrame = (QueryRepeatFrameSigniture)GetProcAddress(dx9DLL, "D3DPERF_QueryRepeatFrame");
		SetOptions = (SetOptionsSigniture)GetProcAddress(dx9DLL, "D3DPERF_SetOptions");
		GetStatus = (GetStatusSigniture)GetProcAddress(dx9DLL, "D3DPERF_GetStatus");

		return true;
	}
}