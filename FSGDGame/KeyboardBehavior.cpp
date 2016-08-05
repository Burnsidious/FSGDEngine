
#include "KeyboardBehavior.h"

#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
#include "../EDEventManager/EventManager.h"
#include "../EDInput/InputManager.h"

using namespace EDInput;
using namespace EDUtilities;
using namespace EDGOCA;
using std::string;

namespace KeyboardBehaviorMessages
{
	DeclareMessage( MSG_INIT_KEYS);
}

namespace Behaviors
{
	KeyboardBehavior::KeyboardBehavior(void)
	{

	}

	void KeyboardBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( KeyboardBehaviorMessages::MSG_INIT_KEYS, OnInitialize ) );

	}

	void KeyboardBehavior::OnInitialize(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		KeyboardBehavior *behaviorPtr = (KeyboardBehavior*)pInvokingBehavior;
		string &fileName = (string &)((MessageT<string> *)msg)->value;

		string temp = ContentManager::theContentPath;
		temp += fileName;
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
		ButtonState stateFromFile;
		InputManager &inputMan = InputManager::GetReference();
		string eventName;

		while(keyElemPtr != 0 && messageElemPtr != 0)
		{
			// Get the message to map to the key
			stringFromFile = messageElemPtr->GetText();
			// Get teh key to map to a message
			keyFromFile = inputMan.GetKeyByName(keyElemPtr->GetText());
			// Get a state of the key to be mapped to a message event
			stateFromFile = inputMan.GetKeyStateByName(stateElemPtr->GetText());

			eventName = keyElemPtr->GetText();
			eventName += '_';
			eventName += stringFromFile;

			inputMan.AddKeyTrigger(keyFromFile, stateFromFile, eventName);

			EDEventManager::EventManager *em = EDEventManager::EventManager::GetInstance();

			UINT id = em->RegisterForEvent(eventName, behaviorPtr, OnKeyCallback);

			MessageNameKey key = ComponentSystem::GetMessageKey(stringFromFile);

			behaviorPtr->events.insert(std::pair<unsigned int, MessageNameKey>(id, key));

			keyElemPtr = keyElemPtr->NextSiblingElement("Key");
			messageElemPtr = messageElemPtr->NextSiblingElement("Message");
			stateElemPtr = stateElemPtr->NextSiblingElement("State");
		}
	}

	void KeyboardBehavior::OnKeyCallback(unsigned int eventId, void *eventData, void *listener)
	{
		KeyboardBehavior *behaviorPtr = (KeyboardBehavior *)listener;

		std::map<unsigned int, MessageNameKey >::iterator item = behaviorPtr->events.find(eventId);
		behaviorPtr->gameObject->OnBroadcastMessage( &MessageT<UINT>((*item).second, 0 ) );
	}

}
