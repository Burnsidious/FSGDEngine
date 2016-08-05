#include "Controller.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"

using namespace EDGOCA;
using namespace EDMath;

namespace ControllerMessages
{
	DeclareMessage( MSG_MOVE_FORWARD );
	DeclareMessage( MSG_MOVE_BACKWARD );
	DeclareMessage( MSG_MOVE_LEFT );
	DeclareMessage( MSG_MOVE_RIGHT );
	DeclareMessage( MSG_TURN_LEFT );
	DeclareMessage( MSG_TURN_RIGHT );
}

namespace Behaviors
{
	void Controller::PopulateMessageMap()
	{
		AddMessageHandler( MessageHandler( ControllerMessages::MSG_MOVE_FORWARD, OnMoveForward ) );
		AddMessageHandler( MessageHandler( ControllerMessages::MSG_MOVE_BACKWARD, OnMoveBackward ) );
		AddMessageHandler( MessageHandler( ControllerMessages::MSG_MOVE_LEFT, OnMoveLeft ) );
		AddMessageHandler( MessageHandler( ControllerMessages::MSG_MOVE_RIGHT, OnMoveRight ) );
		AddMessageHandler( MessageHandler( ControllerMessages::MSG_TURN_LEFT, OnTurnLeft ) );
		AddMessageHandler( MessageHandler( ControllerMessages::MSG_TURN_RIGHT, OnTurnRight ) );
	}

	void Controller::OnMoveForward( IBehavior* invokingBehavior, IMessage* message )
	{
		Controller* pController = (Controller*)invokingBehavior;

		pController->gameObject->TranslateLocal(Float3(0.0f, 0.0f, 0.05f));
	}

	void Controller::OnMoveBackward( IBehavior* invokingBehavior, IMessage* message )
	{
		Controller* pController = (Controller*)invokingBehavior;

		pController->gameObject->TranslateLocal(Float3(0.0f, 0.0f, -0.05f));
	}

	void Controller::OnMoveLeft( IBehavior* invokingBehavior, IMessage* message )
	{
		Controller* pController = (Controller*)invokingBehavior;

		pController->gameObject->TranslateLocal(Float3(-0.05f, 0.0f, 0.0f));
	}

	void Controller::OnMoveRight( IBehavior* invokingBehavior, IMessage* message )
	{
		Controller* pController = (Controller*)invokingBehavior;

		pController->gameObject->TranslateLocal(Float3(0.05f, 0.0f, 0.0f));
	}

	void Controller::OnTurnLeft( IBehavior* invokingBehavior, IMessage* message )
	{
		Controller* pController = (Controller*)invokingBehavior;

		pController->gameObject->RotateLocalY(-0.025f);
	}

	void Controller::OnTurnRight( IBehavior* invokingBehavior, IMessage* message )
	{
		Controller* pController = (Controller*)invokingBehavior;

		pController->gameObject->RotateLocalY(0.025f);
	}
}