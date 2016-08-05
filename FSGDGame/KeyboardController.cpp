#include "KeyboardController.h"

#include "../EDEventManager/EventManager.h"
#include "../EDInput/InputManager.h"
#include "../EDUtilities/ContentManager.h"


using namespace EDUtilities;

void KeyboardController::SetKeyMap(const char* keyMapPath)
{
	path = keyMapPath;
}

void KeyboardController::Awake(void)
{
	string temp = ContentManager::theContentPath;
	temp += path;
	const char* xmlFileNamePtr = temp.c_str();

	TiXmlDocument doc(xmlFileNamePtr);

	if(!doc.LoadFile())
	{
		InternalOutput::GetReference().Error(
			"Failed to load %s, does the file exist?\n", xmlFileNamePtr);
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* elementPtr;
	TiXmlHandle hRoot(0);
		
	elementPtr = hDoc.FirstChildElement().Element();
	if( !elementPtr )
	{
		InternalOutput::GetReference().Error(
			"Failed to load %s, is the file empty?\n", xmlFileNamePtr);
		return;
	}

	std::string rootName = elementPtr->Value();

	if( strcmp(rootName.c_str(), "KeyMapping") != 0 )
	{
		InternalOutput::GetReference().Error(
			"Failed to load %s, missing a KeyMapping element\n", 
			xmlFileNamePtr);
		return;
	}

	TiXmlElement *keyElemPtr = elementPtr->FirstChildElement("Key");
	TiXmlElement *messageElemPtr = elementPtr->FirstChildElement("Message");
	TiXmlElement *stateElemPtr = elementPtr->FirstChildElement("State");

	const char *stringFromFile;
	uint16_t keyFromFile;
	string eventName;

	using namespace EDInput;
	ButtonState stateFromFile;
	unsigned int id = -1;
	EDGameCore::RegistrationId key = -1;
	while(keyElemPtr != 0 && messageElemPtr != 0)
	{
		// Get the message to map to the key
		stringFromFile = messageElemPtr->GetText();
		// Get teh key to map to a message
		keyFromFile = GetKeyByName(keyElemPtr->GetText());
		// Get a state of the key to be mapped to a message event
		stateFromFile = GetKeyStateByName(stateElemPtr->GetText());

		eventName = keyElemPtr->GetText();
		eventName += '_';
		eventName += stringFromFile;

		id = AddInputCallback(KEYBOARD_TYPE, keyFromFile, stateFromFile, eventName, OnKeyCallback, this);
		key = EDGameCore::Message(stringFromFile).messageId;

		this->events.insert(std::pair<unsigned int, EDGameCore::RegistrationId>(id, key));

		keyElemPtr = keyElemPtr->NextSiblingElement("Key");
		messageElemPtr = messageElemPtr->NextSiblingElement("Message");
		stateElemPtr = stateElemPtr->NextSiblingElement("State");
	}
}

void KeyboardController::OnKeyCallback(unsigned int eventId, const void *eventData, void *listener)
{
	KeyboardController *behaviorPtr = (KeyboardController *)listener;

	std::map<unsigned int, EDGameCore::RegistrationId >::iterator item = behaviorPtr->events.find(eventId);
	
	behaviorPtr->GetGameObject()->BroadcastMessage( EDGameCore::Message( (*item).second ) );
}

void KeyboardController::LoadState(TiXmlElement* xmlElement)
{
	assert(xmlElement !=  0);

	TiXmlElement* pathEle = xmlElement->FirstChildElement("Path");

	if( pathEle != 0 &&
		pathEle->GetText() != 0 &&
		strlen( pathEle->GetText() ) != 0 )
		SetKeyMap(pathEle->GetText());
}

void KeyboardController::OnDestroy(void)
{
	while(!events.empty())
	{
		EDEventManager::EventManager::GetInstance()->UnRegisterForEvent( events.begin()->first, this );
		events.erase( events.begin() );
	}
}