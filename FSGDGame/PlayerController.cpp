#include "PlayerController.h"
#include "../EDGameCore/Game.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/RigidBody.h"
#include <functional>

PlayerController::PlayerController(void)
{
	transZ = rotY = 0.0f;
	grounded = false;
	oldPos = Float3(0.0f, 0.0f, 0.0f);
	jump = false;
}

void PlayerController::OnMessage(EDGameCore::Message& message)
{
	static const EDGameCore::RegistrationId MSG_MOVE_FORWARD = EDGameCore::Message("MSG_MOVE_FORWARD").messageId;
	static const EDGameCore::RegistrationId MSG_MOVE_BACKWARD = EDGameCore::Message("MSG_MOVE_BACKWARD").messageId;
	static const EDGameCore::RegistrationId MSG_TURN_LEFT = EDGameCore::Message("MSG_TURN_LEFT").messageId;
	static const EDGameCore::RegistrationId MSG_TURN_RIGHT = EDGameCore::Message("MSG_TURN_RIGHT").messageId;
	static const EDGameCore::RegistrationId MSG_MOVE_UP = EDGameCore::Message("MSG_MOVE_UP").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_CANCEL_Z = EDGameCore::Message("MSG_ROTATE_CANCEL_Z").messageId;

	if( message.messageId == MSG_MOVE_FORWARD )
	{
		transZ = 1.0f;
	}
	else if( message.messageId == MSG_MOVE_BACKWARD )
	{
		transZ = -1.0f;
	}
	else if( message.messageId == MSG_TURN_LEFT )
		rotY = -1.0f;
	else if( message.messageId == MSG_TURN_RIGHT )
		rotY = 1.0f;
	else if( message.messageId == MSG_MOVE_UP )
		jump = true;
	
	if( message.messageId == MSG_ROTATE_CANCEL_Z )
	{
		EDGameCore::GameObject* camObj = GetTransform()->Find("CameraMount");

		if( camObj != 0 )
		{
			EDGameCore::Transform* transform = camObj->GetTransform(); 

			EDGameCore::MessageT< pair<float, float> >& msg = (EDGameCore::MessageT< pair<float, float> >&)message;

			Float4x4 mat = transform->GetLocalMatrix();

			Float3 delta = mat.WAxis;
			mat.WAxis.makeZero();

			mat.rotateLocalX( msg.value.first );
			mat.rotateLocalY( msg.value.second );
		
			CrossProduct( mat.XAxis, Float3(0.0f, 1.0f, 0.0f), mat.ZAxis );
			mat.XAxis.normalize();
			CrossProduct( mat.YAxis, mat.ZAxis, mat.XAxis );
			mat.YAxis.normalize();

			mat.translateLocal( 0.0f, 1.25f, -7.5f );

			transform->SetLocalMatrix(mat);
		}
	}
}

void PlayerController::Update(void)
{
	static const unsigned int terrainMask = EDGameCore::Game::GetLayerMask("Terrain");

	transZ = 0.0f;
	rotY = 0.0f;
	jump = false;
}

void PlayerController::FixedUpdate(void)
{
	static const unsigned int terrainMask = EDGameCore::Game::GetLayerMask("Terrain");
	static const unsigned int crateMask = EDGameCore::Game::GetLayerMask("Crate");
	static const unsigned int ballMask = EDGameCore::Game::GetLayerMask("Ball");
	static const unsigned int playerMask = EDGameCore::Game::GetLayerMask("Player");

	Float3 delta(0.0f, 0.0f, 0.0f);
	//Float3 pos = GetTransform()->GetWorldMatrix().WAxis;
	//Float3 delta = (pos - oldPos) * 0.99f;
	//oldPos = GetGameObject()->GetTransform()->GetWorldMatrix().WAxis;

	//delta.x = delta.z = 0.0f;

	//delta.y += -20.0f * EDGameCore::Game::GetDeltaTime() * EDGameCore::Game::GetDeltaTime();

	float rotAmt = rotY * EDGameCore::Game::GetDeltaTime() * 2.0f;

	float transAmt = transZ * EDGameCore::Game::GetDeltaTime() * 2000.0f;

	if( abs( rotAmt ) > 0.0f )
		GetTransform()->RotateLocalY( rotAmt );
	if( abs( transAmt ) > 0.0f )
	{
		GetTransform()->TranslateLocal( Float3( 0.0f, 0.0f, transZ * EDGameCore::Game::GetDeltaTime() * 4.0f ) );
		GetRigidBody()->Wake();
	}
		//GetRigidBody()->AddForce( GetTransform()->GetWorldMatrix().ZAxis * transAmt );		

		//delta += GetTransform()->GetWorldMatrix().ZAxis * transAmt;

	//GetTransform()->TranslateGlobal( delta );
	
	/*
	Float3 center = GetTransform()->GetWorldMatrix().WAxis;
	center.y += 1.125f;
	EDCollision::Contact contact;

	Capsule capsule;
	capsule.center = center;
	capsule.direction = Float3(0.0f, -1.0f, 0.0f);
	capsule.height = 1.75f;
	capsule.radius = 0.25f;
	
	contact.PenetrationDepth = -FLT_MAX;
	contact.numPoints = 0;

	auto func = [&](EDGameCore::ICollider* collider)
	{
		EDCollision::Contact contact;

		if( collider->Collide( capsule, &contact ) )
		{
			EDGameCore::RigidBody* rigidBody = collider->GetAttachedRigidBody();

			if( (EDGameCore::Game::GetLayerMask( collider->GetGameObject()->GetLayer() ) & (ballMask | playerMask)) != 0 )
			{
				if( rigidBody == nullptr )
					return;
			
				EDGameCore::RigidBody::ResolveCollision( rigidBody, contact, collider->GetMaterialId(), 0 );
			}
			else if( EDGameCore::Game::GetLayerMask( collider->GetGameObject()->GetLayer() ) == terrainMask )
			{
				if( contact.Normal.y > 0.866f )
				{
					contact.Normal.x = contact.Normal.z = 0.0f;
				}
				else if( contact.Normal.y < 0.7071f )
					contact.Normal.y = 0.0f;

				Float3 correction = contact.Normal * contact.PenetrationDepth ;

				this->GetGameObject()->GetTransform()->TranslateGlobal( correction );	
			}
		}
	};

	//EDGameCore::Physics::OverlapTraverser< std::function<void (EDGameCore::ICollider* collider)> >( func, capsule.GetAabb(), ballMask | terrainMask | playerMask );

	//if( jump )
	//{
	//	EDGameCore::RayHit rayHit;
	//	if( EDGameCore::Physics::RayCast( center, Float3(0.0f, -1.0f, 0.0f), 1.25f, terrainMask, rayHit ) )
	//	{
	//		oldPos.y -= 0.15f;
	//	}

	//	jump = false;
	//}
	*/
}

void PlayerController::LateUpdate(void)
{
}

void PlayerController::Awake(void)
{	
	oldPos = GetTransform()->GetWorldMatrix().WAxis;
}