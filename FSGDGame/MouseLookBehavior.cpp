
#include "MouseLookBehavior.h"
#include "WorldData.h"

#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
#include "../EDInput/InputManager.h"

using namespace EDInput;
using namespace EDUtilities;
using namespace EDGOCA;
using std::string;

namespace MouseLookBehaviorMessages
{
	DeclareMessage( MSG_MOUSE_BEGIN_ROTATE);
	DeclareMessage( MSG_MOUSE_END_ROTATE);
	DeclareMessage( MSG_MOUSE_ROTATE);
	DeclareMessage( MSG_UPDATE );
	DeclareMessage( MSG_INIT_MOUSE);
}

namespace Behaviors
{
	void MouseLookBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( MouseLookBehaviorMessages::MSG_MOUSE_BEGIN_ROTATE, OnBeginRotate ) );
		AddMessageHandler( MessageHandler( MouseLookBehaviorMessages::MSG_MOUSE_END_ROTATE, OnEndRotate ) );
		AddMessageHandler( MessageHandler( MouseLookBehaviorMessages::MSG_MOUSE_ROTATE, OnRotate ) );
		AddMessageHandler( MessageHandler( MouseLookBehaviorMessages::MSG_INIT_MOUSE, OnInitialize ) );
	}

	void MouseLookBehavior::OnBeginRotate(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		MouseLookBehavior *behaviorPtr = (MouseLookBehavior*)pInvokingBehavior;
		behaviorPtr->cursorPosX = EDInput::InputManager::GetReference().GetCursorPosition().x;
		behaviorPtr->cursorPosY = EDInput::InputManager::GetReference().GetCursorPosition().y;
		behaviorPtr->rotateEnabled = true;
	}

	void MouseLookBehavior::OnEndRotate(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		MouseLookBehavior *behaviorPtr = (MouseLookBehavior*)pInvokingBehavior;
		behaviorPtr->rotateEnabled = false;
	}

	void MouseLookBehavior::OnRotate(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		assert(reinterpret_cast<MouseLookBehavior *>(pInvokingBehavior));
		MouseLookBehavior *behaviorPtr = (MouseLookBehavior*)pInvokingBehavior;

		// TODO - comment this out and implement your own version
		OnRotateSolution(pInvokingBehavior, msg);
	}

	void MouseLookBehavior::OnInitialize(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		MouseLookBehavior *behaviorPtr = (MouseLookBehavior*)pInvokingBehavior;
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
		ButtonState stateFromFile;
		InputManager &inputMan = InputManager::GetReference();

		while(keyElemPtr != 0 && messageElemPtr != 0)
		{
			// Get the message to map to the key
			stringFromFile = messageElemPtr->GetText();
			// Get teh key to map to a message
			keyFromFile = inputMan.GetKeyByName(keyElemPtr->GetText());
			// Get a state of the key to be mapped to a message event
			stateFromFile = inputMan.GetKeyStateByName(stateElemPtr->GetText());

			inputMan.AddMouseTrigger(keyFromFile, stateFromFile, stringFromFile);

			EDEventManager::EventManager *em = EDEventManager::EventManager::GetInstance();
			UINT id = em->RegisterForEvent(stringFromFile, behaviorPtr, OnMouseEventCallback);
			MessageNameKey key = ComponentSystem::GetMessageKey(stringFromFile);

			behaviorPtr->events.insert(std::pair<unsigned int, MessageNameKey>(id, key));

			keyElemPtr = keyElemPtr->NextSiblingElement("Key");
			stateElemPtr = stateElemPtr->NextSiblingElement("State");
			messageElemPtr = messageElemPtr->NextSiblingElement("Message");
		}
	}

	void MouseLookBehavior::OnMouseEventCallback(unsigned int eventId, void *eventData, void *listener)
	{
		MouseLookBehavior *behaviorPtr = (MouseLookBehavior *)listener;

		std::map<unsigned int, MessageNameKey >::iterator item = behaviorPtr->events.find(eventId);
		behaviorPtr->gameObject->OnMessage( &MessageT<UINT>((*item).second, 0 ) );
	}
}
