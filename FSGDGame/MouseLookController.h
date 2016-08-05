#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"
#include <map>

#ifndef MOUSE_LOOK_SPEED_MODIFIER
#define MOUSE_LOOK_SPEED_MODIFIER 0.0025f
#endif

class MouseLookController : public EDGameCore::Behavior<MouseLookController>
{
public:
	void SetKeyMap(const char* keyMapPath);

private:

	static void OnMouseEventCallback(unsigned int eventId, const void *eventData, void *listener);

	std::map<unsigned int, EDGameCore::RegistrationId > events;

	POINT cursorPos;
	string path;

	void OnMessage(EDGameCore::Message& message);
	void OnMessage(EDGameCore::Message& message, const void * data);
	void LoadState(TiXmlElement* xmlElement);
	void Awake(void);
	void OnDestroy(void);

	void MouseLookControllerSolution(POINT& _new_pos);
};