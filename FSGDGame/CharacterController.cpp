#include "CharacterController.h"
#include "WorldData.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"

using namespace EDGOCA;
using namespace EDMath;
using namespace EDCollision;

#define TIME_STEP 0.01666666f
#define TIME_STEP_SQ (TIME_STEP*TIME_STEP)

namespace CharacterControllerMessages
{
	DeclareMessage( MSG_MOVE_FORWARD );
	DeclareMessage( MSG_MOVE_UP );
	DeclareMessage( MSG_MOVE_BACKWARD );
	DeclareMessage( MSG_TURN_LEFT );
	DeclareMessage( MSG_TURN_RIGHT );
}

namespace
{
	DeclareAttribute(bool, GROUNDED_ATTRIB);
}

namespace Behaviors
{
	const AttributeNameKey* CharacterController::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			GROUNDED_ATTRIB,
			0
		};

		return attribs;
	}

	void CharacterController::OnAdd(void)
	{
		//m_PreviousPosition = gameObject->GetWorldTransform().WAxis;
		Attribute<bool>* groundedAttrib = (Attribute<bool>*)GetAttribute(GROUNDED_ATTRIB);
		groundedAttrib->value = true;
	}

	void CharacterController::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( CharacterControllerMessages::MSG_MOVE_FORWARD, OnMoveForward ) );
		AddMessageHandler( MessageHandler( CharacterControllerMessages::MSG_MOVE_BACKWARD, OnMoveBackward ) );
		AddMessageHandler( MessageHandler( CharacterControllerMessages::MSG_MOVE_UP, OnMoveUp ) );
		AddMessageHandler( MessageHandler( CharacterControllerMessages::MSG_TURN_LEFT, OnTurnLeft ) );
		AddMessageHandler( MessageHandler( CharacterControllerMessages::MSG_TURN_RIGHT, OnTurnRight ) );
	}

	void CharacterController::OnMoveForward( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* controller = (CharacterController*)invokingBehavior;

		Attribute<bool>* groundedAttrib = (Attribute<bool>*)controller->GetAttribute(GROUNDED_ATTRIB);
		if( groundedAttrib->value )
			controller->gameObject->TranslateLocal( Float3( 0.0f, 0.0f, 1.0f )  * controller->ComputeSpeed());
	}

	void CharacterController::OnMoveBackward( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* controller = (CharacterController*)invokingBehavior;

		Attribute<bool>* groundedAttrib = (Attribute<bool>*)controller->GetAttribute(GROUNDED_ATTRIB);
		if( groundedAttrib->value )
			controller->gameObject->TranslateLocal( Float3( 0.0f, 0.0f, -1.0f )  * controller->ComputeSpeed());
	}

	void CharacterController::OnMoveLeft( IBehavior* invokingBehavior, IMessage* message )
	{
	}

	void CharacterController::OnMoveRight( IBehavior* invokingBehavior, IMessage* message )
	{
	}

	void CharacterController::OnTurnLeft( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* pController = (CharacterController*)invokingBehavior;

		pController->gameObject->RotateLocalY(-pController->ComputeRotSpeed());
	}

	void CharacterController::OnTurnRight( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* pController = (CharacterController*)invokingBehavior;

		pController->gameObject->RotateLocalY(pController->ComputeRotSpeed());
	}

	void CharacterController::OnMoveUp( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* controller = (CharacterController*)invokingBehavior;

		Attribute<bool>* groundedAttrib = (Attribute<bool>*)controller->GetAttribute(GROUNDED_ATTRIB);
		if( groundedAttrib->value )
		{
			controller->gameObject->TranslateLocal( Float3( 0.0f, 1.0f, 0.0f )  * controller->ComputeSpeed());
			groundedAttrib->value = false;
		}
	}

	float CharacterController::ComputeSpeed()
	{
		return baseSpeed * WorldData::GetDeltaTime();
	}

	float CharacterController::ComputeRotSpeed()
	{
		return baseRotSpeed * WorldData::GetDeltaTime();
	}
}