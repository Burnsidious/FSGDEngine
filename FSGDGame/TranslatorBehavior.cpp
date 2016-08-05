#include "TranslatorBehavior.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
using namespace EDGOCA;

#include "WorldData.h"

namespace TranslatorBehaviorMessages
{
	DeclareMessage( MSG_MOVE_FORWARD );
	DeclareMessage( MSG_MOVE_BACKWARD );
	DeclareMessage( MSG_MOVE_LEFT );
	DeclareMessage( MSG_MOVE_RIGHT );
	DeclareMessage( MSG_MOVE_UP );
	DeclareMessage( MSG_MOVE_DOWN );
}

namespace Behaviors
{
	void TranslatorBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( TranslatorBehaviorMessages::MSG_MOVE_FORWARD, OnMoveForward ) );
		AddMessageHandler( MessageHandler( TranslatorBehaviorMessages::MSG_MOVE_BACKWARD, OnMoveBackward ) );
		AddMessageHandler( MessageHandler( TranslatorBehaviorMessages::MSG_MOVE_LEFT, OnMoveLeft ) );
		AddMessageHandler( MessageHandler( TranslatorBehaviorMessages::MSG_MOVE_RIGHT, OnMoveRight ) );
		AddMessageHandler( MessageHandler( TranslatorBehaviorMessages::MSG_MOVE_UP, OnMoveUp ) );
		AddMessageHandler( MessageHandler( TranslatorBehaviorMessages::MSG_MOVE_DOWN, OnMoveDown ) );
	}

	void TranslatorBehavior::OnMoveForward( IBehavior* invokingBehavior, IMessage* message )
	{
		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// TODO - comment this out and implement your own version
		OnMoveForwardSolution(invokingBehavior, message);
	}

	void TranslatorBehavior::OnMoveBackward( IBehavior* invokingBehavior, IMessage* message )
	{
		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// TODO - comment this out and implement your own version
		OnMoveBackwardSolution(invokingBehavior, message);
	}

	void TranslatorBehavior::OnMoveLeft( IBehavior* invokingBehavior, IMessage* message )
	{
		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// TODO - comment this out and implement your own version
		OnMoveLeftSolution(invokingBehavior, message);
	}

	void TranslatorBehavior::OnMoveRight( IBehavior* invokingBehavior, IMessage* message )
	{
		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// TODO - comment this out and implement your own version
		OnMoveRightSolution(invokingBehavior, message);
	}

	void TranslatorBehavior::OnMoveUp( IBehavior* invokingBehavior, IMessage* message )
	{
		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// TODO - comment this out and implement your own version
		OnMoveUpSolution(invokingBehavior, message);
	}

	void TranslatorBehavior::OnMoveDown( IBehavior* invokingBehavior, IMessage* message )
	{
		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// TODO - comment this out and implement your own version
		OnMoveDownSolution(invokingBehavior, message);
	}

	float TranslatorBehavior::ComputeSpeed()
	{
		return baseSpeed * WorldData::GetDeltaTime();
	}

}
