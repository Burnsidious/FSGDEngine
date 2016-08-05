#pragma once

#include "../EDGOCA/BehaviorT.h"

namespace Behaviors
{
	class TranslatorBehavior : public EDGOCA::BehaviorT<TranslatorBehavior>
	{
	public:
		TranslatorBehavior(void)
		{
			baseSpeed = 7.5f;
		}

		~TranslatorBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnMoveForward( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveBackward( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveLeft( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnMoveRight( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnMoveUp( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnMoveDown( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );

		static void OnMoveForwardSolution( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveBackwardSolution( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveLeftSolution( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveRightSolution( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveUpSolution( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveDownSolution( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );


	private:
		float baseSpeed;

		float ComputeSpeed();
	};
}
