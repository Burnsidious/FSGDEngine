#pragma once

#include "../EDGameCore/Behavior.h"

class GamepadController : public EDGameCore::Behavior<GamepadController>
{
public:
	void SetKeyMap(const char* keyMapPath);

private:

	void Awake(void);
	void OnDestroy(void);

	static void OnKeyCallback(unsigned int eventId, const void *eventData, void *listener);
	
	std::map<unsigned int, EDGameCore::RegistrationId> events;

	void LoadState(TiXmlElement* xmlElement);

	string path;
};