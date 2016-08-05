#include "Crosshair.h"

#include "../EDGameCore/Game.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/Message.h"
#include "HardAttach.h"

void Crosshair::OnMessage(EDGameCore::Message& message)
{
	static const EDGameCore::RegistrationId targetNameHash = EDGameCore::Game::GetStringRegistrationId("Target");
	static const EDGameCore::RegistrationId MSG_CLAMP = EDGameCore::Message("MSG_CLAMP").messageId;

	if( message.messageId == MSG_CLAMP )
	{
		auto& msg = (EDGameCore::MessageT<unsigned int>&)message;

		EDGameCore::GameObject* clampToObj = EDGameCore::GameObject::GetGameObjectInstance( msg.value );

		if( clampToObj == 0 )
			return;

		if( clampToObj->GetNameHash() == targetNameHash )
		{
			Float4x4 newLocal = clampToObj->GetTransform()->GetWorldMatrix();

			GetTransform()->SetLocalMatrix( newLocal );
		}
	}
}