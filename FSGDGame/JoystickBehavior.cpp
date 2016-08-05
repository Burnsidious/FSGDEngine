#include "JoystickBehavior.h"
#include "WorldData.h"

#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
#include "../EDEventManager/EventManager.h"
#include "../EDInput/JoystickManager.h"


using namespace EDInput;
using namespace EDMath;
using namespace EDUtilities;
using namespace EDGOCA;
using std::string;
using std::pair;
using std::map;
namespace
{
	DeclareMessage( MSG_INIT_JOYSTICK);
	DeclareMessage( MSG_LEFT_JOYSTICK);
	DeclareMessage( MSG_RIGHT_JOYSTICK);
}

namespace Behaviors
{
	JoystickBehavior::JoystickBehavior(void)
	{
		translateSpeed = 7.5f;
		rotateSpeed = 4.0f;
	}

	void JoystickBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( MSG_INIT_JOYSTICK, OnInitialize ) );
		AddMessageHandler( MessageHandler( MSG_LEFT_JOYSTICK, OnLeftStick) );
	}

	void JoystickBehavior::OnInitialize(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		JoystickBehavior *behaviorPtr = (JoystickBehavior*)pInvokingBehavior;
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

		if( strcmp(rootName.c_str(), "JoystickMapping") != 0 )
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
		BUTTON_STATE stateFromFile;
		JoystickManager &inputMan = JoystickManager::GetReference();
		string eventName;

		while(keyElemPtr != 0 && messageElemPtr != 0)
		{
			// Get the message to map to the key
			stringFromFile = messageElemPtr->GetText();
			// Get the key to map to a message
			keyFromFile = inputMan.GetKeyByName(keyElemPtr->GetText());
			// Get a state of the key to be mapped to a message event
			stateFromFile = inputMan.GetKeyStateByName(stateElemPtr->GetText());

			eventName = keyElemPtr->GetText();
			eventName += '_';
			eventName += stringFromFile;

			inputMan.AddJoystickTrigger(keyFromFile, eventName, stateFromFile);

			EDEventManager::EventManager *em = EDEventManager::EventManager::GetInstance();

			UINT id = em->RegisterForEvent(eventName, behaviorPtr, OnInputCallback);

			MessageNameKey key = ComponentSystem::GetMessageKey(stringFromFile);

			behaviorPtr->events.insert(pair<unsigned int, MessageNameKey>(id, key));

			keyElemPtr = keyElemPtr->NextSiblingElement("Key");
			messageElemPtr = messageElemPtr->NextSiblingElement("Message");
			stateElemPtr = stateElemPtr->NextSiblingElement("State");
		}
	}

	void JoystickBehavior::OnInputCallback(unsigned int eventId, void *eventData, void *listener)
	{
		JoystickBehavior *behaviorPtr = (JoystickBehavior *)listener;

		map<unsigned int, MessageNameKey >::iterator item = behaviorPtr->events.find(eventId);
		behaviorPtr->gameObject->OnBroadcastMessage( &MessageT<void *>((*item).second, eventData ) );
	}

	void JoystickBehavior::OnLeftStick(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		JoystickBehavior *behaviorPtr = (JoystickBehavior*)pInvokingBehavior;
		PlayerInputReport *eventData = (PlayerInputReport *)((MessageT<void *> *)msg)->value;

		float fForwardTranslate = 0.0f;
		float fLateralTranslate = 0.0f;

		if (eventData)
		{
			if (eventData->dataPtr)
			{
				fForwardTranslate = ((float*)eventData->dataPtr)[1];
				fLateralTranslate = ((float*)eventData->dataPtr)[0];
			}
		}

		behaviorPtr->gameObject->TranslateLocal( Float3( 0.0f, 0.0f, fForwardTranslate ) * behaviorPtr->ComputeTranslateSpeed()  );
		behaviorPtr->gameObject->RotateLocalY(fLateralTranslate * behaviorPtr->ComputeRotateSpeed());
	}

	float JoystickBehavior::ComputeTranslateSpeed()
	{
		return translateSpeed * WorldData::GetDeltaTime();
	}

	float JoystickBehavior::ComputeRotateSpeed()
	{
		return rotateSpeed * WorldData::GetDeltaTime();
	}
}
