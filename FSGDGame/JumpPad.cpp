#include "JumpPad.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDGameCore/RigidBody.h"
void JumpPad::Update(void)
{
	EDGameCore::ICollider* colliderPtr = GetCollider();

	if( colliderPtr != 0 )
	{
		EDRendererD3D::DebugRenderer::GetInstance()->DrawAabb( colliderPtr->GetAabb().Min(), colliderPtr->GetAabb().Max() );
	}
}

void JumpPad::OnTriggerExit(EDGameCore::ICollider* thisCollider, EDGameCore::ICollider* otherCollider, const EDCollision::Contact& contact)
{
	EDGameCore::RigidBody* rigidBodyPtr = otherCollider->GetAttachedRigidBody();

	if( rigidBodyPtr != 0 )
	{
		EDCollision::Contact c = contact;
		c.negate();

		rigidBodyPtr->AddForce( Float3( 0.0f, 1000.0f, 1000.0f ) );
		//rigidBodyPtr->ApplyForceAtPoint( Float3( 0.0f, 1000.0f, 1000.0f ), c.Point[0] );
	}
}