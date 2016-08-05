#include "HardAttachBehavior.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"

using namespace EDMath;
using namespace EDGOCA;

namespace HardAttachMessages
{
	DeclareMessage( MSG_ATTACH );
	DeclareMessage( MSG_SET_OFFSET );
}

namespace Behaviors
{
	void HardAttachBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( HardAttachMessages::MSG_ATTACH, OnAttach ) );
		AddMessageHandler( MessageHandler( HardAttachMessages::MSG_SET_OFFSET, OnSetOffset ) );

	}

	void HardAttachBehavior::OnAttach(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		HardAttachBehavior *attachBehavior = (HardAttachBehavior*)pInvokingBehavior;
		MessageT<GameObject *> *message = (MessageT<GameObject *>*)msg;
		//message->value->AddChild(attachBehavior->gameObject);
		message->value->AddChildObject(attachBehavior->gameObject, -1);
	}

	void HardAttachBehavior::OnSetOffset(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		HardAttachBehavior *attachBehavior = (HardAttachBehavior*)pInvokingBehavior;
		MessageT<Float3 *> *message = (MessageT<Float3 *>*)msg;
		attachBehavior->gameObject->SetLocalPosition(*message->value);
	}
}