#include "BuggyDriver.h"

#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/RigidBody.h"
#include "../EDGameCore/Collider.h"
#include "../EDGameCore/Game.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDGameCore/LayerManager.h"
#include "../EDGameCore/Camera.h"
#include "../EDGameCore/WheelCollider.h"
#include "../EDGameCore/Animation.h"

const float BuggyDriver::BULLET_FIRE_INTERVAL = 0.1f;

BuggyDriver::BuggyDriver(void)
{
	transZ = 0.0f;
	rotY = 0.0f;
	steeringAngle = 0.0f;
	gunMoved = 0;
	lastFiredTime = 0.0f;
	translationSpeed = 2;
	rotationSpeed = 2;

	for(int i = 0; i < BULLET_MAX; ++i)
		bulletCache.push_back( &bullets[i] );
}

void BuggyDriver::Update(void)
{
	if( rotY > 1.0f )
		rotY = 1.0f;
	else if( rotY < -1.0f )
		rotY = -1.0f;
	else
		rotY = 0.0f;

	if( transZ > 1.0f )
		transZ = 1.0f;
	else if( transZ < -1.0f )
		transZ = -1.0f;
	else
		transZ = 0.0f;

	if( gunMoved <= 0 )
	{
		gunMoved = 0;

		EDGameCore::Transform* transform = GetTransform()->Find("Gun")->GetTransform();
		float rotY = DotProduct( transform->GetWorldMatrix().XAxis, GetTransform()->GetWorldMatrix().ZAxis ) * EDGameCore::Game::GetDeltaTime();
		float rotX = DotProduct( transform->GetWorldMatrix().YAxis, GetTransform()->GetWorldMatrix().ZAxis ) * EDGameCore::Game::GetDeltaTime();

		transform->RotateLocalX( -rotX );
		transform->RotateLocalY( rotY );

		Float4x4 mat = transform->GetLocalMatrix();
		
		CrossProduct( mat.XAxis, Float3(0.0f, 1.0f, 0.0f), mat.ZAxis );
		mat.XAxis.normalize();
		CrossProduct( mat.YAxis, mat.ZAxis, mat.XAxis );
		mat.YAxis.normalize();

		transform->SetLocalMatrix(mat);
	}

	auto bulletIter = liveBullets.begin();
	while (bulletIter != liveBullets.end())
	{
		auto next = bulletIter;
		++next;

		(*bulletIter)->age += EDGameCore::Game::GetDeltaTime();
		if( (*bulletIter)->age > 1.0f )
		{
			bulletCache.push_back( (*bulletIter) );
			liveBullets.erase( bulletIter );
		}
		else
		{
			EDRendererD3D::VERTEX_POSCOLOR start;
			EDRendererD3D::VERTEX_POSCOLOR end;
			
			end.color = start.color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
			start.position = DirectX::XMFLOAT3( (*bulletIter)->oldPos.v );
			end.position = DirectX::XMFLOAT3( (*bulletIter)->pos.v );

			EDRendererD3D::DebugRenderer::GetInstance()->DrawSegment( start, end );
		}
		
		bulletIter = next;
	}

	lastFiredTime += EDGameCore::Game::GetDeltaTime();

	if( lastFiredTime > BULLET_FIRE_INTERVAL )
	{
		EDGameCore::Transform* transform = GetTransform()->Find("Gun")->GetTransform();
		EDGameCore::GameObject* muzzle = transform->Find("MuzzleFlare");
	
		muzzle->GetLight()->disable();
	}	

	//Sphere searchSphere;
	//searchSphere.center = GetTransform()->GetWorldMatrix().WAxis;
	//searchSphere.radius = 3.0f;

	//auto func = [&](EDGameCore::ICollider* collider)
	//{
	//	if( collider->GetAttachedRigidBody() != 0 &&
	//		collider->GetAttachedRigidBody()->GetGameObject() == GetGameObject() )
	//		return;

	//	EDGameCore::Game::destroy( collider->GetAttachedRigidBody()->GetGameObject() );
	//};

	//EDGameCore::Game::OverlapBoundingVolume( searchSphere, func );

	/*
	EDGameCore::GameObject* targetObj = 0;

	if( targetID != 0 )
		targetObj = EDGameCore::Game::GetGameObjectInstance(targetID);
	else
	{
		Sphere searchSphere;
		searchSphere.center = GetTransform()->GetWorldMatrix().WAxis;
		searchSphere.radius = 10.0f;

		auto func = [&](EDGameCore::ICollider* collider)
		{
		};
	}
	*/

	//EDRendererD3D::DebugRenderer::GetInstance()->DrawAabb( GetCollider()->GetAabb().Min(), GetCollider()->GetAabb().Max() );
}

void BuggyDriver::OnMessage(EDGameCore::Message& message, const void *data)
{
	static const EDGameCore::RegistrationId MSG_MOVE = EDGameCore::Message("MSG_MOVE").messageId;
	static const EDGameCore::RegistrationId MSG_ACCELERATE = EDGameCore::Message("MSG_ACCELERATE").messageId;
	static const EDGameCore::RegistrationId MSG_FIRE = EDGameCore::Message("MSG_FIRE").messageId;

	if (message.messageId == MSG_MOVE)
	{
		EDGameCore::Transform* transform = GetGameObject()->GetTransform();
		const float * info = static_cast<const float *>(data);
		float deltaTime = EDGameCore::Game::GetDeltaTime();


		//float forwardSpeed = 10.0f * info[1] * deltaTime;
		//float sideSpeed = 10.0f * info[0] * deltaTime;

		//transform->TranslateLocal(Float3(0.0f, 0.0f, forwardSpeed));
		//transZ = translationSpeed;// 2 * info[1];
		rotY = rotationSpeed * info[0];
	}
	//else if (message.messageId == MSG_ACCELERATE)
	//{
	//	transZ = -translationSpeed;
	//}
}

void BuggyDriver::OnMessage(EDGameCore::Message& message)
{
	static const EDGameCore::RegistrationId MSG_MOVE_FORWARD = EDGameCore::Message("MSG_MOVE_FORWARD").messageId;
	static const EDGameCore::RegistrationId MSG_MOVE_BACKWARD = EDGameCore::Message("MSG_MOVE_BACKWARD").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_LEFT = EDGameCore::Message("MSG_ROTATE_LEFT").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_RIGHT = EDGameCore::Message("MSG_ROTATE_RIGHT").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_UP = EDGameCore::Message("MSG_ROTATE_UP").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_DOWN = EDGameCore::Message("MSG_ROTATE_DOWN").messageId;
	static const EDGameCore::RegistrationId MSG_TURN_LEFT = EDGameCore::Message("MSG_TURN_LEFT").messageId;
	static const EDGameCore::RegistrationId MSG_TURN_RIGHT = EDGameCore::Message("MSG_TURN_RIGHT").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_CANCEL_Z = EDGameCore::Message("MSG_ROTATE_CANCEL_Z").messageId;
	static const EDGameCore::RegistrationId MSG_FIRE = EDGameCore::Message("MSG_FIRE").messageId;
	static const EDGameCore::RegistrationId MSG_TARGET_LOCKED = EDGameCore::Message("MSG_TARGET_LOCKED").messageId;

	if( message.messageId == MSG_MOVE_FORWARD )
	{
		transZ = translationSpeed;
		gunMoved -= 1;
	}
	else if( message.messageId == MSG_MOVE_BACKWARD )
	{
		transZ = -translationSpeed;
		gunMoved -= 1;
	}
	else if( message.messageId == MSG_TURN_LEFT )
		rotY = -rotationSpeed;
	else if( message.messageId == MSG_TURN_RIGHT )
		rotY = rotationSpeed;
	else if( message.messageId == MSG_ROTATE_CANCEL_Z )
	{
		EDGameCore::Transform* transform = GetTransform()->Find("Gun")->GetTransform();

		EDGameCore::MessageT< pair<float, float> >& msg = (EDGameCore::MessageT< pair<float, float> >&)message;

		transform->RotateLocalX( msg.value.first );
		transform->RotateLocalY( msg.value.second );

		Float4x4 mat = transform->GetLocalMatrix();
		
		if( mat.ZAxis.y > 0.923879f )
		{
			mat.ZAxis.y = 0.923879f;
			mat.ZAxis.normalize();
		}
		else if( mat.ZAxis.y < -0.7071f )
		{
			mat.ZAxis.y = -0.7071f;
			mat.ZAxis.normalize();
		}

		CrossProduct( mat.XAxis, Float3(0.0f, 1.0f, 0.0f), mat.ZAxis );
		mat.XAxis.normalize();
		CrossProduct( mat.YAxis, mat.ZAxis, mat.XAxis );
		mat.YAxis.normalize();

		transform->SetLocalMatrix(mat);

		gunMoved = 2;
	}
	else if( message.messageId == MSG_FIRE || message.messageId == MSG_TARGET_LOCKED )
	{
		if( !bulletCache.empty() )
		{
			if( lastFiredTime >= BULLET_FIRE_INTERVAL )
			{
				EDGameCore::Transform* transform = GetTransform()->Find("Gun")->GetTransform();
				EDGameCore::GameObject* muzzle = transform->Find("MuzzleFlare");

				lastFiredTime = 0.0f;
				
				bullet* b = bulletCache.front();
				bulletCache.pop_front();

				b->age = 0.0f;
				b->pos = muzzle->GetTransform()->GetWorldMatrix().WAxis;
				b->oldPos = b->pos - transform->GetWorldMatrix().ZAxis * 1.0;

				liveBullets.push_back( b );

				muzzle->GetLight()->enable();
			}
		}
	}

	GetRigidBody()->Wake();
}

void BuggyDriver::FixedUpdate(void)
{
	static const EDGameCore::RegistrationId targetNameHash = EDGameCore::Game::GetStringRegistrationId("Target");
	
	EDGameCore::Transform* transform = GetGameObject()->GetTransform();
	EDGameCore::RigidBody* rigidBody = GetGameObject()->GetRigidBody();

	EDGameCore::GameObject* lwObj = transform->Find("RLWheel");
	EDGameCore::GameObject* rwObj = transform->Find("RRWheel");
	EDGameCore::WheelCollider* wcl = (EDGameCore::WheelCollider*)lwObj->GetCollider();
	EDGameCore::WheelCollider* wcr = (EDGameCore::WheelCollider*)rwObj->GetCollider();	

	//const double MAX_SPIN = 3.14159 * 0.1f;

	
	float newSpin = atanf(wcl->GetMotorTorque() / wcl->GetRadius());
	//if( wcl->GetMotorTorque() < 0.0f )
	//	newSpin = -newSpin;

	//wcl->SetWheelSpin(newSpin );
	//wcr->SetWheelSpin(newSpin);

	lwObj->GetTransform()->RotateLocalX( newSpin );

	newSpin = atanf(wcr->GetMotorTorque() / wcr->GetRadius());
	rwObj->GetTransform()->RotateLocalX( newSpin );

	wcl->SetMotorTorque( wcl->GetMotorTorque() * 0.95f );
	wcr->SetMotorTorque( wcr->GetMotorTorque() * 0.95f );

	lwObj = transform->Find("FLWheel");
	rwObj = transform->Find("FRWheel");
	wcl = (EDGameCore::WheelCollider*)lwObj->GetCollider();
	wcr = (EDGameCore::WheelCollider*)rwObj->GetCollider();	

	newSpin = atanf(wcl->GetMotorTorque() / wcl->GetRadius());
	lwObj->GetTransform()->RotateLocalX( newSpin );
	newSpin = atanf(wcr->GetMotorTorque() / wcr->GetRadius());
	rwObj->GetTransform()->RotateLocalX( newSpin );

	//wcl->SetWheelSpin(newSpin);
	//wcr->SetWheelSpin(newSpin);

	wcl->SetMotorTorque( wcl->GetMotorTorque() * 0.95f );
	wcr->SetMotorTorque( wcr->GetMotorTorque() * 0.95f );

	const float speed = 0.0375f;
	if( transZ*transZ > 0.0f )
	{
		lwObj = transform->Find("FLWheel");
		rwObj = transform->Find("FRWheel");

		wcl = (EDGameCore::WheelCollider*)lwObj->GetCollider();
		wcl->SetMotorTorque( transZ * speed);

		wcr = (EDGameCore::WheelCollider*)rwObj->GetCollider();	
		wcr->SetMotorTorque( transZ * speed);

		lwObj = transform->Find("RLWheel");
		rwObj = transform->Find("RRWheel");

		wcl = (EDGameCore::WheelCollider*)lwObj->GetCollider();
		wcl->SetMotorTorque( transZ * speed);

		wcr = (EDGameCore::WheelCollider*)rwObj->GetCollider();	
		wcr->SetMotorTorque( transZ * speed);

		rigidBody->Wake();
	}

	lwObj = transform->Find("FLWheel");
	rwObj = transform->Find("FRWheel");

	if( rotY < 0.0f )
	{
		steeringAngle -= 0.1f;

		if( steeringAngle < -0.7854f )
			steeringAngle = -0.7854f;
	}
	else if( rotY > 0.0f )
	{
		steeringAngle += 0.1f;

		if( steeringAngle > 0.7854f )
			steeringAngle = 0.7854f;
	}
	else
	{
		steeringAngle *= 0.95f;
	}

	Float4x4 yRotMat;
	yRotMat.makeRotationY( steeringAngle );
	Float4x4 yRotMat2;
	yRotMat2.makeRotationY( steeringAngle * 0.5f );

	if( steeringAngle < 0.0f )
	{
		yRotMat.WAxis = lwObj->GetTransform()->GetLocalMatrix().WAxis;
		yRotMat2.WAxis = rwObj->GetTransform()->GetLocalMatrix().WAxis;

		yRotMat.ZAxis = lwObj->GetTransform()->GetLocalMatrix().ZAxis;	
		CrossProduct( yRotMat.YAxis, yRotMat.ZAxis, yRotMat.XAxis );
		yRotMat.YAxis.normalize();
		CrossProduct( yRotMat.ZAxis, yRotMat.XAxis, yRotMat.YAxis );
		yRotMat.ZAxis.normalize();
		
		yRotMat2.ZAxis = rwObj->GetTransform()->GetLocalMatrix().ZAxis;
		CrossProduct( yRotMat2.YAxis, yRotMat2.ZAxis, yRotMat2.XAxis );
		yRotMat2.YAxis.normalize();
		CrossProduct( yRotMat2.ZAxis, yRotMat2.XAxis, yRotMat2.YAxis );
		yRotMat2.ZAxis.normalize();

		lwObj->GetTransform()->SetLocalMatrix(yRotMat);
		rwObj->GetTransform()->SetLocalMatrix(yRotMat2);
	}
	else
	{
		yRotMat.WAxis = rwObj->GetTransform()->GetLocalMatrix().WAxis;
		yRotMat2.WAxis = lwObj->GetTransform()->GetLocalMatrix().WAxis;

		yRotMat.ZAxis = rwObj->GetTransform()->GetLocalMatrix().ZAxis;	
		CrossProduct( yRotMat.YAxis, yRotMat.ZAxis, yRotMat.XAxis );
		yRotMat.YAxis.normalize();
		CrossProduct( yRotMat.ZAxis, yRotMat.XAxis, yRotMat.YAxis );
		yRotMat.ZAxis.normalize();

		yRotMat2.ZAxis = lwObj->GetTransform()->GetLocalMatrix().ZAxis;
		CrossProduct( yRotMat2.YAxis, yRotMat2.ZAxis, yRotMat2.XAxis );
		yRotMat2.YAxis.normalize();
		CrossProduct( yRotMat2.ZAxis, yRotMat2.XAxis, yRotMat2.YAxis );
		yRotMat2.ZAxis.normalize();

		rwObj->GetTransform()->SetLocalMatrix(yRotMat);
		lwObj->GetTransform()->SetLocalMatrix(yRotMat2);
	}

	//static const unsigned int crateMask = EDGameCore::Game::GetLayerMask("Crate");
	//static const unsigned int terrainMask = EDGameCore::Game::GetLayerMask("Terrain");
	//static const unsigned int ballMask = EDGameCore::Game::GetLayerMask("Ball");
	//static const unsigned int playerMask = EDGameCore::Game::GetLayerMask("Soldier");

	auto bulletIter = liveBullets.begin();
	while (bulletIter != liveBullets.end())
	{
		auto next = bulletIter;
		++next;

		bullet* b = (*bulletIter);

		Float3 delta = (b->pos - b->oldPos);
		float deltaLen = delta.magnitude();

		Float3 rayNorm = delta * (1.0f / deltaLen);
		EDGameCore::RayHit rayHit;
		if( EDGameCore::Physics::RayCast( b->pos, rayNorm, deltaLen, EDGameCore::Game::GetLayerMask(GetGameObject()->GetLayer() ), rayHit) )
		{
			if( rayHit.collider != 0 && rayHit.collider != GetCollider() && rayHit.collider->GetAttachedRigidBody() )
			{
				EDGameCore::RigidBody* rigidBody = rayHit.collider->GetAttachedRigidBody();
				
				rayHit.collider->GetAttachedRigidBody()->ApplyForceAtPoint( delta * 1000000.0f, rayHit.Point );
			}
			else if( rayHit.collider != 0 && rayHit.collider != GetCollider() && rayHit.collider->GetGameObject()->GetNameHash() == targetNameHash )
			{
				rayHit.collider->GetGameObject()->OnMessage( EDGameCore::Message("MSG_SPIN") );
			}

			//if( rayHit.collider != 0 && rayHit.collider->GetAttachedRigidBody() != 0 )
			//{
			//	EDGameCore::RigidBody* rigidBody = rayHit.collider->GetAttachedRigidBody();

			//	rigidBody->SetAngularFactor(1.0f);
			//	rigidBody->SetLinearFactors( Float3(1.0f, 1.0f, 1.0f ) );

			//	rayHit.collider->GetAttachedRigidBody()->ApplyForceAtPoint( delta * 1000.0f, rayHit.Point );
			//
			//	EDGameCore::Animation* anim = rayHit.collider->GetAnimation();

			//	if( anim != 0 )
			//		anim->disable();
			//}

			liveBullets.erase( bulletIter );
			bulletCache.push_back( b );
		}
		else
		{
			Float3 newPos = b->pos + (b->pos - b->oldPos);
			b->oldPos = b->pos;
			b->pos = newPos;
		}

		bulletIter = next;
	}

	//EDGameCore::GameObject* gunObj = GetTransform()->Find("Gun");
	//EDGameCore::GameObject* cameraObj = gunObj->GetTransform()->Find("CameraMount");

	//EDGameCore::Camera* cam = cameraObj->GetComponent<EDGameCore::Camera>();

	//Float3 start;
	//Float3 normal;
	//
	//cam->ScreenPointToRay(start, normal, 0.5f, 0.5f);

	//EDGameCore::RayHit rayHit;
	//if( EDGameCore::Physics::RayCast( gunObj->GetTransform()->GetWorldMatrix().WAxis, normal, FLT_MAX, crateMask | terrainMask | ballMask, rayHit ) )
	//{
	//	if( rayHit.collider != 0 && rayHit.collider->GetAttachedRigidBody() != 0 )
	//	{
	//		rayHit.collider->GetAttachedRigidBody()->ApplyForceAtPoint( normal * 100.0f, rayHit.Point );
	//	}
	//}
	//EDGameCore::Physics::SphereCast( GetTransform()->GetWorldMatrix().WAxis, 1.0f, crateMask | ballMask, onSphereCast, GetGameObject() );
}