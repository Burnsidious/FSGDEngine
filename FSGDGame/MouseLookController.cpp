#include "MouseLookController.h"

#include "../EDEventManager/EventManager.h"
#include "../EDInput/InputManager.h"
#include "../EDUtilities/ContentManager.h"
#include "../EDGameCore/Transform.h"
#include "../EDUtilities/InternalOutput.h"
#include "../EDInput/Source/MouseState.h"

using namespace EDUtilities;


void MouseLookController::SetKeyMap(const char* keyMapPath)
{
	path = keyMapPath;
}

void MouseLookController::Awake(void)
{
	string temp = ContentManager::theContentPath;
	temp += string(path);
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

	if( strcmp(rootName.c_str(), "MouseMapping") != 0 )
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

		id = AddInputCallback(MOUSE_TYPE, keyFromFile, stateFromFile, stringFromFile, OnMouseEventCallback, this);
		key = EDGameCore::Message(stringFromFile).messageId;

		this->events.insert(std::pair<unsigned int, EDGameCore::RegistrationId>(id, key));

		keyElemPtr = keyElemPtr->NextSiblingElement("Key");
		stateElemPtr = stateElemPtr->NextSiblingElement("State");
		messageElemPtr = messageElemPtr->NextSiblingElement("Message");
	}
}

void MouseLookController::OnMouseEventCallback(unsigned int eventId, const void *eventData, void *listener)
{
	MouseLookController *behaviorPtr = (MouseLookController *)listener;

	if( behaviorPtr->GetGameObject() && behaviorPtr->GetGameObject()->isEnabledInHierarchy() )
	{
		std::map<unsigned int, EDGameCore::RegistrationId >::iterator item = behaviorPtr->events.find(eventId);
		if (nullptr == eventData)
		{
			behaviorPtr->GetGameObject()->OnMessage( EDGameCore::Message( (*item).second ) );
		}
		else
		{
			behaviorPtr->GetGameObject()->OnMessage( EDGameCore::Message( (*item).second ), eventData);
		}
	}
}
void MouseLookController::OnMessage(EDGameCore::Message& message)
{
	static const EDGameCore::RegistrationId MSG_MOUSE_END_ROTATE = EDGameCore::Message("MSG_MOUSE_END_ROTATE").messageId;
	
	if( message.messageId == MSG_MOUSE_END_ROTATE )
	{
		bool state = false;
		EDEventManager::EventManager::GetInstance()->PostEvent("Stop Mouse", &state, EDEventManager::EM_PRIORITY_HIGH, EDEventManager::EM_FLAG_IMMEDIATE);
		state = true;
		EDEventManager::EventManager::GetInstance()->PostEvent("Show Cursor", &state, EDEventManager::EM_PRIORITY_HIGH, EDEventManager::EM_FLAG_IMMEDIATE);
	}
}
void MouseLookController::OnMessage(EDGameCore::Message& message, const void * data)
{
	static const EDGameCore::RegistrationId MSG_MSG_MOUSE_BEGIN_ROTATE = EDGameCore::Message("MSG_MOUSE_BEGIN_ROTATE").messageId;
	static const EDGameCore::RegistrationId MSG_MOUSE_ROTATE = EDGameCore::Message("MSG_MOUSE_ROTATE").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_CANCEL_Z = EDGameCore::Message("MSG_ROTATE_CANCEL_Z").messageId;

	if( message.messageId == MSG_MSG_MOUSE_BEGIN_ROTATE )
	{
		bool state = false;
		cursorPos = *static_cast<const POINT *>(data);
		EDEventManager::EventManager::GetInstance()->PostEvent("Show Cursor", &state, EDEventManager::EM_PRIORITY_HIGH, EDEventManager::EM_FLAG_IMMEDIATE);
	}
	else if( message.messageId == MSG_MOUSE_ROTATE )
	{
		float fRotateX = 0.0f, fRotateY = 0.0f;
		POINT new_point = *static_cast<const POINT *>(data);
		Float4x4 camera_mat = GetGameObject()->GetTransform()->GetLocalMatrix();
		
		// TODO - comment this out and write your own solution
		MouseLookControllerSolution(new_point);

		EDEventManager::EventManager::GetInstance()->PostEvent("Set Cursor", &cursorPos, EDEventManager::EM_PRIORITY_HIGH, EDEventManager::EM_FLAG_IMMEDIATE);
	}
}

void MouseLookController::LoadState(TiXmlElement* xmlElement)
{
	assert(xmlElement !=  0);

	TiXmlElement* pathEle = xmlElement->FirstChildElement("Path");

	if( pathEle != 0 &&
		pathEle->GetText() != 0 &&
		strlen( pathEle->GetText() ) != 0 )
		SetKeyMap(pathEle->GetText());
}

void MouseLookController::OnDestroy(void)
{
	while(!events.empty())
	{
		EDEventManager::EventManager::GetInstance()->UnRegisterForEvent( events.begin()->first, this );
		events.erase( events.begin() );
	}
}

