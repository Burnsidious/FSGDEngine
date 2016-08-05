#include "ExplodeOnCollide.h"
#include "../EDGameCore/Game.h"
#include "../EDGameCore/RigidBody.h"

void ExplodeOnCollide::OnCollisionEnter(EDGameCore::ICollider* thisCollider, EDGameCore::ICollider* otherCollider, const EDCollision::Contact& contact)
{
	static const EDGameCore::RegistrationId targetNameHash = EDGameCore::Game::GetStringRegistrationId("PlayerBuggy");

	if( otherCollider->GetAttachedRigidBody() == 0 )
		return;

	if( otherCollider->GetAttachedRigidBody()->GetGameObject()->GetNameHash() == targetNameHash )
	{
		EDGameCore::Game::Instantiate( "Blast", GetTransform()->GetWorldMatrix() );
		EDGameCore::Game::destroy( GetGameObject() );
	}
}