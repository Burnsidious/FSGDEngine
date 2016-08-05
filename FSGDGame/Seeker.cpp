#include "Seeker.h"

#include "../EDGameCore/Game.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/RigidBody.h"
#include "../EDCollision/sphere.h"

#include <functional>

Seeker::Seeker(void)
{
	targetTag = 0;
	targetId = 0;
}

void Seeker::LoadState(TiXmlElement* xmlElement)
{
	TiXmlElement* targetLayer = xmlElement->FirstChildElement("TargetTag");
	
	if( targetLayer->GetText() != 0 &&
		strlen( targetLayer->GetText() ) != 0 )
			targetTag = (unsigned int)std::hash< std::string >()( targetLayer->GetText() );

	TiXmlElement* trackingEle = xmlElement->FirstChildElement("TrackingJoint");

	if( trackingEle != 0 &&
		trackingEle->GetText() != 0 &&
		strlen( trackingEle->GetText() ) != 0 )
	{
		trackingJoint = trackingEle->GetText();
	}
	else
		trackingJoint = "";
}

void Seeker::Update(void)
{
	static const EDGameCore::RegistrationId MSG_TURN_LEFT = EDGameCore::Message("MSG_TURN_LEFT").messageId;
	static const EDGameCore::RegistrationId MSG_TURN_RIGHT = EDGameCore::Message("MSG_TURN_RIGHT").messageId;
	static const EDGameCore::RegistrationId MSG_MOVE_FORWARD = EDGameCore::Message("MSG_MOVE_FORWARD").messageId;
	static const EDGameCore::RegistrationId MSG_ROTATE_CANCEL_Z = EDGameCore::Message("MSG_ROTATE_CANCEL_Z").messageId;
	static const EDGameCore::RegistrationId MSG_FIRE = EDGameCore::Message("MSG_FIRE").messageId;

	if( targetId == 0 )
		return;

	EDGameCore::GameObject* targetObject = EDGameCore::Game::GetGameObjectInstance(targetId);

	if( targetObject != 0 )
	{
		targetId = targetObject->GetInstanceId();

		Float3 Delta = targetObject->GetTransform()->GetWorldMatrix().WAxis - GetTransform()->GetWorldMatrix().WAxis;

		float dotRes = DotProduct( GetTransform()->GetWorldMatrix().XAxis, Delta );

		if( dotRes > 0.0f )
			GetGameObject()->OnMessage( EDGameCore::Message(MSG_TURN_RIGHT) );
		else if( dotRes < 0.0f )
			GetGameObject()->OnMessage( EDGameCore::Message(MSG_TURN_LEFT) );

		if( trackingJoint.length() != 0 )
		{
			EDGameCore::GameObject* trackingObj = GetTransform()->Find(trackingJoint.c_str());

			if( trackingObj != 0 )
			{
				Float3 vToTarget = targetObject->GetTransform()->GetWorldMatrix().WAxis - trackingObj->GetTransform()->GetWorldMatrix().WAxis;

				if( targetObject->GetCollider() != 0 )
				{
					if( targetObject->GetCollider()->GetAttachedRigidBody() != 0 )
					{
						Float3 center;
						TransformPoint( center, targetObject->GetCollider()->GetAttachedRigidBody()->GetCenterOfMass(), targetObject->GetCollider()->GetAttachedRigidBody()->GetTransform()->GetWorldMatrix() );
						vToTarget =  center - trackingObj->GetTransform()->GetWorldMatrix().WAxis;
					}
				}

				EDMath::InvTransformVector( vToTarget, vToTarget, trackingObj->GetTransform()->GetWorldMatrix() );

				float deltaY = -vToTarget.y * 0.0025f;
				float deltaX = vToTarget.x * 0.0025f;
				
				if( abs( deltaY ) > 0.005f || abs( deltaX ) > 0.005f )
				{
					EDGameCore::MessageT<pair<float,float>> rotMsg(MSG_ROTATE_CANCEL_Z, make_pair(-vToTarget.y * 0.0025f, vToTarget.x * 0.0025f));
					GetGameObject()->OnMessage( rotMsg );
				}
				else
				{
					EDGameCore::MessageT<unsigned int> lockMsg( "MSG_TARGET_LOCKED", targetId );
					GetGameObject()->OnMessage( lockMsg );
				}
			}
		}

		GetGameObject()->OnMessage( EDGameCore::Message( MSG_MOVE_FORWARD ) );
	}

	targetId = 0;
}

void Seeker::OnTriggerStay(EDGameCore::ICollider* thisCollider, EDGameCore::ICollider* otherCollider, const EDCollision::Contact& contact)
{
	if( !otherCollider->GetGameObject()->HasTag(targetTag) )
		return;

	if( targetId == 0 )
	{
		targetId = otherCollider->GetGameObject()->GetInstanceId();
		return;
	}

	EDGameCore::GameObject* targetObj = EDGameCore::Game::GetGameObjectInstance(targetId);

	if( targetObj == 0 )
	{
		targetId = otherCollider->GetGameObject()->GetInstanceId();
		return;
	}

	float currentDelta = (GetTransform()->GetWorldMatrix().WAxis - targetObj->GetTransform()->GetWorldMatrix().WAxis).squaredMagnitude();
	float newDelta = (GetTransform()->GetWorldMatrix().WAxis - otherCollider->GetTransform()->GetWorldMatrix().WAxis).squaredMagnitude();

	if( newDelta < currentDelta )
	{
		targetId = otherCollider->GetGameObject()->GetInstanceId();
		return;		
	}
}