#include "Mover.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/RigidBody.h"
#include "../EDGameCore/Collider.h"
#include "../EDGameCore/Game.h"
#include "../EDInput/RegisterInformation.h"

void Mover::OnMessage(EDGameCore::Message& message)
{
	static const EDGameCore::RegistrationId MSG_MOVE_FORWARD = EDGameCore::Message("MSG_MOVE_FORWARD").messageId;
	static const EDGameCore::RegistrationId MSG_MOVE_BACKWARD = EDGameCore::Message("MSG_MOVE_BACKWARD").messageId;
	static const EDGameCore::RegistrationId MSG_MOVE_LEFT = EDGameCore::Message("MSG_MOVE_LEFT").messageId;
	static const EDGameCore::RegistrationId MSG_MOVE_RIGHT = EDGameCore::Message("MSG_MOVE_RIGHT").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_LEFT = EDGameCore::Message("MSG_ROTATE_LEFT").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_RIGHT = EDGameCore::Message("MSG_ROTATE_RIGHT").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_UP = EDGameCore::Message("MSG_ROTATE_UP").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_DOWN = EDGameCore::Message("MSG_ROTATE_DOWN").messageId;
	static const EDGameCore::RegistrationId MSG_TURN_LEFT = EDGameCore::Message("MSG_TURN_LEFT").messageId;
	static const EDGameCore::RegistrationId MSG_TURN_RIGHT = EDGameCore::Message("MSG_TURN_RIGHT").messageId;
	static const EDGameCore::RegistrationId MSG_MOVE_UP = EDGameCore::Message("MSG_MOVE_UP").messageId;
	static const EDGameCore::RegistrationId MSG_MOVE_DOWN = EDGameCore::Message("MSG_MOVE_DOWN").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_CANCEL_Z = EDGameCore::Message("MSG_ROTATE_CANCEL_Z").messageId;

	EDGameCore::Transform* transform = GetGameObject()->GetTransform();

	EDGameCore::RigidBody* rigidBody = GetRigidBody();
	if( rigidBody )
		rigidBody->Wake();

	float deltaTime = EDGameCore::Game::GetDeltaTime();
	float moveSpeed = 10.0f * deltaTime;
	float rotSpeed = 5.0f * deltaTime;


	if( message.messageId == MSG_MOVE_FORWARD )
		transform->TranslateLocal(Float3(0.0f, 0.0f, moveSpeed));
	else if( message.messageId == MSG_MOVE_BACKWARD )
		transform->TranslateLocal(Float3(0.0f, 0.0f, -moveSpeed));
	else if( message.messageId == MSG_MOVE_LEFT )
		transform->TranslateLocal(Float3(-moveSpeed, 0.0f, 0.0f));
	else if( message.messageId == MSG_MOVE_RIGHT )
		transform->TranslateLocal(Float3(moveSpeed, 0.0f, 0.0f));
	else if (message.messageId == MSG_MOVE_UP)
		transform->TranslateLocal(Float3(0.0f, moveSpeed, 0.0f));
	else if (message.messageId == MSG_MOVE_DOWN)
		transform->TranslateLocal(Float3(0.0f, -moveSpeed, 0.0f));
	else if (message.messageId == MSG_ROTATE_LEFT || message.messageId == MSG_TURN_LEFT)
		transform->RotateLocalY(-rotSpeed);
	else if( message.messageId == MSG_ROTATE_RIGHT || message.messageId == MSG_TURN_RIGHT )
		transform->RotateLocalY(rotSpeed);
	else if( message.messageId == MSG_ROTATE_UP )
		transform->RotateLocalX(-rotSpeed);
	else if( message.messageId == MSG_ROTATE_DOWN )
		transform->RotateLocalX(rotSpeed);
	
	else if( message.messageId == MSG_ROTATE_CANCEL_Z )
	{
		EDGameCore::MessageT< pair<float, float> >& msg = (EDGameCore::MessageT< pair<float, float> >&)message;
		EDGameCore::Transform* transform = GetGameObject()->GetTransform();

		transform->RotateLocalX(msg.value.first);
		transform->RotateLocalY(msg.value.second);

		Float4x4 mat = transform->GetLocalMatrix();
		
		CrossProduct( mat.XAxis, Float3(0.0f, 1.0f, 0.0f), mat.ZAxis );
		mat.XAxis.normalize();
		CrossProduct( mat.YAxis, mat.ZAxis, mat.XAxis );
		mat.YAxis.normalize();

		transform->SetLocalMatrix(mat);
	}
}
void Mover::OnMessage(EDGameCore::Message& message, const void * data)
{
	static const EDGameCore::RegistrationId MSG_MOVE = EDGameCore::Message("MSG_MOVE").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE = EDGameCore::Message("MSG_ROTATE").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_CANCEL_Z = EDGameCore::Message("MSG_ROTATE_CANCEL_Z").messageId;

	EDGameCore::Transform* transform = GetGameObject()->GetTransform();

	EDGameCore::RigidBody* rigidBody = GetRigidBody();
	if( rigidBody )
		rigidBody->Wake();

	if (nullptr == data)
		return;

	const float * info = static_cast<const float *>(data);
	float deltaTime = EDGameCore::Game::GetDeltaTime();
	
	if(message.messageId == MSG_MOVE)
	{
		float forwardSpeed = 10.0f * info[1] * deltaTime;
		float sideSpeed = 10.0f * info[0] * deltaTime;

		transform->TranslateLocal(Float3(sideSpeed, 0.0f, forwardSpeed) );
	}
	else if(message.messageId == MSG_ROTATE)
	{
		EDMath::Float4x4 mat = transform->GetLocalMatrix();
		float rotY = info[1] * deltaTime;
		float rotX = info[0] * deltaTime;

		transform->RotateLocalY(rotX);
		transform->RotateLocalX(rotY);

		GetGameObject()->OnMessage( EDGameCore::MessageT<pair<float,float>>(MSG_ROTATE_CANCEL_Z, make_pair(rotY, rotX) ) );
	}
}