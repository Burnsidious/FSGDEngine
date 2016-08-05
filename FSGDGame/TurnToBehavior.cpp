#include "TurnToBehavior.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
#include "../FSGDGame/WorldData.h"

using namespace EDGOCA;

namespace TurnToBehaviorMessages
{
	DeclareMessage( MSG_SET_TARGET );
	DeclareMessage( MSG_UPDATE );
}

namespace Behaviors
{
	void TurnToBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( TurnToBehaviorMessages::MSG_SET_TARGET, OnSetTarget ) );
		AddMessageHandler( MessageHandler( TurnToBehaviorMessages::MSG_UPDATE, OnUpdate ) );
	}

	void TurnToBehavior::OnSetTarget(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		assert(reinterpret_cast<TurnToBehavior *>(pInvokingBehavior));
		assert(reinterpret_cast<MessageT<GameObject *> *>(msg));

		TurnToBehavior *turnToPtr = (TurnToBehavior *)pInvokingBehavior;
		MessageT<GameObject *> *message = (MessageT<GameObject *>*)msg;

		turnToPtr->targetObjPtr = message->value;
	}

	void TurnToBehavior::OnUpdate(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		assert(reinterpret_cast<TurnToBehavior *>(pInvokingBehavior));
		TurnToBehavior *turnToPtr = (TurnToBehavior *)pInvokingBehavior;
		
		if( turnToPtr->targetObjPtr == 0 )
			return;

		// TODO - comment this out and implement your own version
		OnUpdateSolution(pInvokingBehavior, msg);
	}
}
