#include "HardAttach.h"

#include "../EDGameCore/Game.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/Message.h"
#include "../EDUtilities/XMLLoadingMethods.h"

void HardAttach::Update(void)
{
	static const EDGameCore::RegistrationId MSG_CLAMP = EDGameCore::Message("MSG_CLAMP").messageId;

	if( targetId != 0 )
	{
		EDGameCore::GameObject* targetObj = EDGameCore::GameObject::GetGameObjectInstance(targetId);

		if( targetObj == 0 )
		{
			targetId = 0;
			return;
		}

		Float4x4 newMat = GetTransform()->GetLocalMatrix();

		TransformPoint( newMat.WAxis, offset, targetObj->GetTransform()->GetWorldMatrix() );
		
		unsigned int clampToId = 0;
		if( clamp )
		{
			Float3 startPos = targetObj->GetTransform()->GetWorldMatrix().WAxis;
			Float3 delta = newMat.WAxis - startPos;
	
			float deltaLen = delta.magnitude();
	
			Float3 deltaN = delta * (1.0f / deltaLen);

			EDGameCore::RayHit rayHit;
			if( EDGameCore::Physics::RayCast( startPos, deltaN, deltaLen, GetGameObject()->GetLayerMask(), rayHit ) )
			{
				newMat.WAxis = rayHit.Point;
				clampToId = rayHit.collider->GetGameObject()->GetInstanceId();
			}
		}
		
		GetTransform()->SetLocalMatrix( newMat );

		if( clampToId != 0 )
			GetGameObject()->OnMessage( EDGameCore::MessageT<unsigned int>(MSG_CLAMP, clampToId) );
	}
}

void HardAttach::Awake(void)
{
	EDGameCore::GameObject* gameObject = EDGameCore::GameObject::Find( targetName.c_str() );

	targetId = gameObject->GetInstanceId();
}

void HardAttach::LoadState(TiXmlElement* xmlElement)
{
	targetId = 0;
	offset.makeZero();
	clamp = false;

	TiXmlElement* offsetEle = xmlElement->FirstChildElement("Offset");

	if( offsetEle != 0 )
		ReadXmlVector3( offsetEle, offset.v );

	TiXmlElement* targetEle = xmlElement->FirstChildElement("TargetName");

	if( targetEle != 0 )
		ReadXmlString( targetEle, targetName );

	TiXmlElement* clampEle = xmlElement->FirstChildElement("Clamp");

	if( clampEle != 0 )
	{
		clamp = true;

		if( clampEle->GetText() != 0 &&
			strlen( clampEle->GetText() ) != 0 &&
			(_stricmp( clampEle->GetText(), "0" ) == 0 || _stricmp( clampEle->GetText(), "false" ) ) )
		{
			clamp = false;
		}
	}
}