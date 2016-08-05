#include "FollowObserver.h"
#include "../EDGameCore/Game.h"
#include "../EDGameCore/Transform.h"

void FollowObserver::SetTargetByTag(const char* tag)
{
	targetTag = tag;
	std::list<EDGameCore::GameObject*> objList;

	EDGameCore::Game::FindObjectsByTag(tag, objList);
	
	if( !objList.empty() )
		targetId = (*objList.begin())->GetInstanceId();
	else
		targetId = 0;
}

void FollowObserver::LoadState(TiXmlElement* xmlElement)
{
	TiXmlElement* targetTagEle = xmlElement->FirstChildElement("TargetTag");

	if( targetTagEle != 0 &&
		targetTagEle->GetText() != 0 &&
		strlen( targetTagEle->GetText() ) != 0 )
		SetTargetByTag( targetTagEle->GetText() );
}

void FollowObserver::Update(void)
{
	if( targetTag.length() != 0 && targetId == 0 )
	{
		std::list<EDGameCore::GameObject*> objList;

		EDGameCore::Game::FindObjectsByTag(targetTag.c_str(), objList);
	
		if( !objList.empty() )
			targetId = (*objList.begin())->GetInstanceId();
		else
			targetId = 0;
	}

	EDGameCore::GameObject* targetObj = 0;

	if( targetId != 0 )
		targetObj = EDGameCore::Game::GetGameObjectInstance(targetId);

	if( targetObj == 0 )
		return;

	Float4x4 newMat = GetTransform()->GetWorldMatrix();

	float minY = targetObj->GetTransform()->GetWorldMatrix().WAxis.y + 2.0f;

	if( newMat.WAxis.y < minY )
		newMat.WAxis.y = minY;

	Float3 delta = targetObj->GetTransform()->GetWorldMatrix().WAxis - newMat.WAxis;

	float mag = delta.magnitude();
	newMat.ZAxis = delta * (1.0f / mag);
	CrossProduct( newMat.XAxis, Float3(0.0f, 1.0f, 0.0f), newMat.ZAxis );
	newMat.XAxis.normalize();
	CrossProduct( newMat.YAxis, newMat.ZAxis, newMat.XAxis );
	newMat.YAxis.normalize();
		
	if( mag > 7.0f )
		newMat.WAxis = targetObj->GetTransform()->GetWorldMatrix().WAxis - newMat.ZAxis * 7.0f;
		
	newMat.WAxis -= targetObj->GetTransform()->GetWorldMatrix().WAxis;;

	float dot = DotProduct( targetObj->GetTransform()->GetWorldMatrix().XAxis, newMat.ZAxis );

	newMat.rotateGlobalY(-dot * 0.01f);

	newMat.WAxis += targetObj->GetTransform()->GetWorldMatrix().WAxis;

	GetTransform()->SetLocalMatrix( newMat );
}