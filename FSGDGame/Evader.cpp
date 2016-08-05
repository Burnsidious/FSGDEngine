#include "Evader.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Message.h"
#include "../EDGameCore/Game.h"

Evader::Evader(void)
{
	evadingId = 0;
	evadingTag = 0;
}

void Evader::LoadState(TiXmlElement* xmlElement)
{
	TiXmlElement* targetLayer = xmlElement->FirstChildElement("EvadingTag");
	
	if( targetLayer->GetText() != 0 &&
		strlen( targetLayer->GetText() ) != 0 )
			evadingTag = (unsigned int)std::hash< std::string >()( targetLayer->GetText() );
}

void Evader::Update(void)
{
	static const EDGameCore::RegistrationId MSG_TURN_LEFT = EDGameCore::Message("MSG_TURN_LEFT").messageId;
	static const EDGameCore::RegistrationId MSG_TURN_RIGHT = EDGameCore::Message("MSG_TURN_RIGHT").messageId;
	static const EDGameCore::RegistrationId MSG_MOVE_FORWARD = EDGameCore::Message("MSG_MOVE_FORWARD").messageId;

	if( evadingId == 0 )
		return;

	EDGameCore::GameObject* evadingObj = EDGameCore::Game::GetGameObjectInstance(evadingId);

	if( evadingObj != 0 )
	{
		Float3 delta = GetTransform()->GetWorldMatrix().WAxis - evadingObj->GetTransform()->GetWorldMatrix().WAxis;

		float dotRes = DotProduct( GetTransform()->GetWorldMatrix().XAxis, delta );

		if( dotRes > 0.0f )
			GetGameObject()->OnMessage( EDGameCore::Message(MSG_TURN_RIGHT) );
		else if( dotRes < 0.0f )
			GetGameObject()->OnMessage( EDGameCore::Message(MSG_TURN_LEFT) );

		GetGameObject()->OnMessage( EDGameCore::Message( MSG_MOVE_FORWARD ) );
	}

	evadingId = 0;
}

void Evader::OnTriggerStay(EDGameCore::ICollider* thisCollider, EDGameCore::ICollider* otherCollider, const EDCollision::Contact& contact)
{
	if( !otherCollider->GetGameObject()->HasTag(evadingTag) )
		return;

	if( evadingId == 0 )
	{
		evadingId = otherCollider->GetGameObject()->GetInstanceId();
		return;
	}

	EDGameCore::GameObject* evadingObj = EDGameCore::Game::GetGameObjectInstance(evadingId);

	if( evadingObj == 0 )
	{
		evadingId = otherCollider->GetGameObject()->GetInstanceId();
		return;
	}

	float currentDelta = (GetTransform()->GetWorldMatrix().WAxis - evadingObj->GetTransform()->GetWorldMatrix().WAxis).squaredMagnitude();
	float newDelta = (GetTransform()->GetWorldMatrix().WAxis - otherCollider->GetTransform()->GetWorldMatrix().WAxis).squaredMagnitude();

	if( newDelta < currentDelta )
	{
		evadingId = otherCollider->GetGameObject()->GetInstanceId();
		return;		
	}
}

