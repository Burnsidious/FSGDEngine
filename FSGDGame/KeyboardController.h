#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"
#include <map>

class KeyboardController : public EDGameCore::Behavior<KeyboardController>
{
public:
	void SetKeyMap(const char* keyMapPath);

private:

	//void OnEnable(void);

	void Awake(void);

	void OnDestroy(void);

	static void OnKeyCallback(unsigned int eventId, const void *eventData, void *listener);
	
	std::map<unsigned int, EDGameCore::RegistrationId> events;

	void LoadState(TiXmlElement* xmlElement);

	string path;
};