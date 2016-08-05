#pragma once

#include "../EDGOCA/BehaviorT.h"

namespace Behaviors
{
	class Controller : public EDGOCA::BehaviorT<Controller>
	{
	public:	
		Controller(void){}
	
		static void PopulateMessageMap(void);

		static void OnMoveForward( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveBackward( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnMoveLeft( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnMoveRight( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnTurnLeft( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
		static void OnTurnRight( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* message );
	};
}