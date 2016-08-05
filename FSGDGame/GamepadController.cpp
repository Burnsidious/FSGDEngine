#include "./GamepadController.h"

#include "../EDEventManager/EventManager.h"
#include "../EDInput/InputManager.h"
#include "../EDUtilities/ContentManager.h"
#include "../EDGameCore/Message.h"

using namespace EDUtilities;

void GamepadController::SetKeyMap(const char* keyMapPath)
{
	path = keyMapPath;
}

void GamepadController::Awake(void)
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
	TiXmlElement *userElemPtr = elementPtr->FirstChildElement("User");

	const char *stringFromFile;
	uint16_t keyFromFile;
	string eventName;

	using namespace EDInput;
	ButtonState stateFromFile;
	GAME_PAD_USER userFromFile;
	unsigned int id = -1;
	EDGameCore::RegistrationId key = -1;
	while(keyElemPtr != 0 && messageElemPtr != 0)
	{
		// Get the message to map to the key
		stringFromFile = messageElemPtr->GetText();
		// Get teh key to map to a message
		keyFromFile = GetKeyByName(keyElemPtr->GetText() );
		// Get a state of the key to be mapped to a message event
		stateFromFile = GetKeyStateByName(stateElemPtr->GetText() );
		//get what controller to use
		userFromFile = GetUserByName(userElemPtr->GetText() );

		id = AddInputCallback(GAME_PAD_TYPE, keyFromFile, stateFromFile, stringFromFile, OnKeyCallback, this, BASIC_INPUT, userFromFile);
		key = EDGameCore::Message(stringFromFile).messageId;

		this->events.insert(std::pair<unsigned int, EDGameCore::RegistrationId>(id, key));

		//TODO how can you keep a table together even if some values are duplicated?
		//userElemPtr = elementPtr->NextSiblingElement("User");
		keyElemPtr = keyElemPtr->NextSiblingElement("Key");
		messageElemPtr = messageElemPtr->NextSiblingElement("Message");
		stateElemPtr = stateElemPtr->NextSiblingElement("State");
		
	}	
}

void GamepadController::OnKeyCallback(unsigned int eventId, const void *eventData, void *listener)
{
	GamepadController *behaviorPtr = (GamepadController *)listener;

	std::map<unsigned int, EDGameCore::RegistrationId >::iterator item = behaviorPtr->events.find(eventId);
	
	behaviorPtr->GetGameObject()->OnMessage( EDGameCore::Message( (*item).second ), eventData);
}

void GamepadController::LoadState(TiXmlElement* xmlElement)
{
	assert(xmlElement !=  0);

	TiXmlElement* pathEle = xmlElement->FirstChildElement("Path");

	if( pathEle != 0 &&
		pathEle->GetText() != 0 &&
		strlen( pathEle->GetText() ) != 0 )
		SetKeyMap(pathEle->GetText());
}

void GamepadController::OnDestroy(void)
{
	while(!events.empty())
	{
		EDEventManager::EventManager::GetInstance()->UnRegisterForEvent( events.begin()->first, this );
		events.erase( events.begin() );
	}
}