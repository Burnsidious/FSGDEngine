#include "TurretBehavior.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
using namespace EDGOCA;

#include "../EDInput/JoystickManager.h"
using namespace EDInput;

#include "WorldData.h"

namespace TurretBehaviorMessages
{
	DeclareMessage( MSG_ROTATE_RIGHT );
	DeclareMessage( MSG_ROTATE_LEFT );
	DeclareMessage( MSG_RIGHT_JOYSTICK);
}

namespace Behaviors
{
	float TurretBehavior::ComputeRotateSpeed()
	{
		return baseSpeed * WorldData::GetDeltaTime();
	}

	void TurretBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( TurretBehaviorMessages::MSG_ROTATE_RIGHT, OnRotateRight ) );
		AddMessageHandler( MessageHandler( TurretBehaviorMessages::MSG_ROTATE_LEFT, OnRotateLeft ) );
		AddMessageHandler( MessageHandler( TurretBehaviorMessages::MSG_RIGHT_JOYSTICK, OnRightStick ) );
	}

	void TurretBehavior::OnRotateRight( IBehavior* invokingBehavior, IMessage* message )
	{
		TurretBehavior* controller = (TurretBehavior*)invokingBehavior;

		controller->gameObject->RotateLocalY(-controller->ComputeRotateSpeed());
	}

	void TurretBehavior::OnRotateLeft( IBehavior* invokingBehavior, IMessage* message )
	{
		TurretBehavior* controller = (TurretBehavior*)invokingBehavior;

		controller->gameObject->RotateLocalY(controller->ComputeRotateSpeed());
	}

	void TurretBehavior::OnRightStick(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		TurretBehavior *behaviorPtr = (TurretBehavior*)pInvokingBehavior;
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

		behaviorPtr->gameObject->RotateLocalY(fLateralTranslate * behaviorPtr->ComputeRotateSpeed());
	}
};
