#include "WheelBehavior.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
using namespace EDGOCA;

#include "WorldData.h"

namespace WheelBehaviorMessages
{
	DeclareMessage( MSG_MOVE_FORWARD );
	DeclareMessage( MSG_MOVE_BACKWARD );
}

namespace Behaviors
{
	void WheelBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( WheelBehaviorMessages::MSG_MOVE_FORWARD, OnMoveForward ) );
		AddMessageHandler( MessageHandler( WheelBehaviorMessages::MSG_MOVE_BACKWARD, OnMoveBackward ) );
	}

	void WheelBehavior::OnMoveForward( IBehavior* invokingBehavior, IMessage* message )
	{
		WheelBehavior* controller = (WheelBehavior*)invokingBehavior;

		controller->gameObject->RotateLocalX(-controller->baseSpeed * WorldData::GetDeltaTime());
	}

	void WheelBehavior::OnMoveBackward( IBehavior* invokingBehavior, IMessage* message )
	{
		WheelBehavior* controller = (WheelBehavior*)invokingBehavior;

		controller->gameObject->RotateLocalX(controller->baseSpeed * WorldData::GetDeltaTime());
	}
};
