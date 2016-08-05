#pragma once
#include "RenderController.h"

#include "../EDRendererD3D/PostProcess.h"

class PauseEffect
{
public:
	PauseEffect(void);
	~PauseEffect(void);

	void Initialize();
	void Activate();
	void Deactivate();

	static void PauseEffectUpdate(unsigned int eventId, const void *eventData, void *listener);
private:
	EDRendererD3D::PostProcess pauseFadePostProcess;

	float maxScale;
	float maxFade;
};


