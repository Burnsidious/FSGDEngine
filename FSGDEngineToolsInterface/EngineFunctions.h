#ifndef ENGINE_FUNCTIONS_H
#define ENGINE_FUNCTIONS_H

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

extern "C" {

	__declspec(dllexport) void InitializeRenderer(HWND windowHandle);
	__declspec(dllexport) void InitializeScene(void);
	__declspec(dllexport) void ShutdownRenderer(void);
	__declspec(dllexport) void RenderMainView(void);
}

#endif //ENGINE_FUNCTIONS_H