#include "Target.h"

#include "../EDGameCore/Game.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "LookAt.h"

void Target::Update(void)
{
	if( spinTimer > 0.0f )
	{
		float deltaTime = EDGameCore::Game::GetDeltaTime();

		spinTimer -= deltaTime;

		GetTransform()->RotateLocalY( 3.14159f * deltaTime );
	}
	else
	{
		LookAt* lookAtBehavior = GetGameObject()->GetBehavior<LookAt>();
		if( lookAtBehavior != 0 )
			lookAtBehavior->enable();
	}
}

void Target::OnMessage(EDGameCore::Message& message)
{
	static const EDGameCore::RegistrationId MSG_SPIN = EDGameCore::Message("MSG_SPIN").messageId;

	if( message.messageId == MSG_SPIN )
	{
		spinTimer = 3.0f;

		LookAt* lookAtBehavior = GetGameObject()->GetBehavior<LookAt>();
		if( lookAtBehavior != 0 )
			lookAtBehavior->disable();
	}
}