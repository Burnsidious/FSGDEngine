#include "Blast.h"

#include "../EDGameCore/Game.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/RigidBody.h"
#include "../EDGameCore/SphereCollider.h"
#include "../EDRendererD3D/DebugRenderer.h"

void Blast::Update(void)
{
	const float LIFE_SPAN = 0.375f;
	static float GREEN = 1.0f;
	
	if( GREEN > 0.5f )
		GREEN = 0.5f;
	else
		GREEN = 1.0f;

	EDGameCore::ICollider* iCollider = GetCollider();
	if( iCollider == 0 )
		return;

	if( iCollider->GetColliderType() != EDGameCore::SPHERE )
		return;

	EDGameCore::SphereCollider* sphereCollider = (EDGameCore::SphereCollider*)iCollider;

	EDMath::Sphere sphere = sphereCollider->GetSphere();

	float deltaTime = EDGameCore::Game::GetDeltaTime();
	age += deltaTime;

	if( age >= LIFE_SPAN )
	{
		EDGameCore::Game::destroy( GetGameObject() );
		return;
	}

	float ratio = age / LIFE_SPAN;

	float ALPHA = 1.0f - ratio*ratio;
	
	sphere.radius *= 1.0f + 10.0f * deltaTime;

	sphereCollider->SetSphere( sphere );

	XMFLOAT4 color( 1.0f, GREEN, 0.0f, ALPHA );

	EDRendererD3D::DebugRenderer::GetInstance()->DrawSphere( sphereCollider->GetWorldSphere(), color );
}

void Blast::OnTriggerStay(EDGameCore::ICollider* thisCollider, EDGameCore::ICollider* otherCollider, const EDCollision::Contact& contact)
{
	if( otherCollider->GetAttachedRigidBody() != 0 )
	{
		EDCollision::Contact contact2 = contact;
		
		Float3 delta = otherCollider->GetTransform()->GetWorldMatrix().WAxis - GetTransform()->GetWorldMatrix().WAxis;

		if( DotProduct( delta, contact.Normal ) < 0.0f )
			contact2.negate();

		float mag = delta.magnitude();

		if( mag == 0.0f )
		{
			delta = Float3(0.0f, 1.0f, 0.0f);
			mag = 1.0f;
		}
		else
			delta *= (1.0f / mag);

		delta *= 15000.0f / (mag*mag);
		delta *= (1.0f / EDGameCore::Game::GetDeltaTime());

		otherCollider->GetAttachedRigidBody()->ApplyForceAtPoint( delta, contact2.Point[0] );
	}
}

void Blast::Awake(void)
{
	age = 0.0f;
}
