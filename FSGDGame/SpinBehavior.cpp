#include "SpinBehavior.h"
#include "WorldData.h"

#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"

using namespace EDGOCA;
using namespace EDMath;

namespace SpinBehaviorMessages
{
	DeclareMessage( MSG_UPDATE );
	DeclareMessage( MSG_SET_LOCAL );
}

namespace Behaviors
{
	void SpinBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( SpinBehaviorMessages::MSG_UPDATE, OnUpdate ) );
		AddMessageHandler( MessageHandler( SpinBehaviorMessages::MSG_SET_LOCAL, OnSetMatrix ) );
	}

	void SpinBehavior::OnUpdate( IBehavior* invokingBehavior, IMessage* message )
	{
		SpinBehavior* controller = (SpinBehavior*)invokingBehavior;

		controller->gameObject->RotateLocalY(controller->speed * WorldData::GetDeltaTime());
	}

	void SpinBehavior::OnSetMatrix( IBehavior* invokingBehavior, IMessage* msg )
	{
		SpinBehavior* controller = (SpinBehavior*)invokingBehavior;

		Float4x4 &mat = ((MessageT<Float4x4 &> *)msg)->value;

		controller->gameObject->SetLocalTransform(mat);
	}

}
