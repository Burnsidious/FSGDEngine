#include "PauseEffect.h"
#include "WorldData.h"
#include "PostProcessController.h"
#include "../EDEventManager/EventManager.h"
#include "../EDRendererD3D/Renderer.h"
using namespace EDRendererD3D;

PauseEffect::PauseEffect(void)
{
	maxScale = 0.5;
	maxFade = 0.75f;
}

PauseEffect::~PauseEffect(void)
{
}

void PauseEffect::Initialize()
{
	pauseFadePostProcess.Initialize(RenderController::GetInstance()->GetScreenQuadPtr(), 
		"2DPassThrough_VS.cso",	"PauseEffect_PS.cso");

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"PauseEffectUpdate", this, PauseEffectUpdate);
}

void PauseEffect::Activate()
{
	PostProcessController::GetInstance()->PushBackPostProcess(
		&pauseFadePostProcess);

	EDEventManager::EventManager::GetInstance()->PostEvent("PauseEffectUpdate", 0, 
		EDEventManager::EM_PRIORITY_NORMAL, EDEventManager::EM_FLAG_PERSISTENT);

	cbPostProcess &postData = PostProcessController::GetInstance()->GetPostShaderData();

	postData.gScale = 0;
	postData.gFade = 1.0f;
	postData.gTheta = 0;
}

void PauseEffect::Deactivate()
{
	PostProcessController::GetInstance()->RemovePostProcess(&pauseFadePostProcess);
	EDEventManager::EventManager::GetInstance()->DeletePersistentEvent("PauseEffectUpdate", 0);
}

void PauseEffect::PauseEffectUpdate(unsigned int eventId, const void *eventData, void *listener)
{
	PauseEffect *pauseEffect = (PauseEffect *)listener;
	cbPostProcess &postData = PostProcessController::GetInstance()->GetPostShaderData();

	postData.gTheta += 
		(WorldData::GetDeltaTime() * postData.gScale / pauseEffect->maxScale);

	if(pauseEffect->maxScale > postData.gScale)
	{
		postData.gScale += WorldData::GetDeltaTime();

		postData.gFade = 1.0f - 
			( pauseEffect->maxFade * (postData.gScale / pauseEffect->maxScale));
	}
}

