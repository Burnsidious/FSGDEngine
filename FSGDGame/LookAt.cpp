#include "LookAt.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/RigidBody.h"
#include "../EDGameCore/Collider.h"
#include "../EDGameCore/Game.h"


#include <list>
#include "../EDGameCore/GameObject.h"

using namespace std;
using namespace EDGameCore;

LookAt::LookAt()
{
	target = nullptr;
	targetId = 0;
}

void LookAt::LateUpdate()
{
	Transform* looker_transform = GetGameObject()->GetTransform();
	Transform* target_transform = nullptr;

	// Finds the target based on its instance id each update 
	// to avoid having a dangling pointer
	// to an object that might have been destroyed.
	target = GameObject::GetGameObjectInstance(targetId);
		

	// TODO - comment this out and write your own solution
	LookAtSolution();
}

void LookAt::LoadState(TiXmlElement* xmlElement)
{
	// Get the name of the target to look-at
	TiXmlElement* targetEle = xmlElement->FirstChildElement("TargetName");
	
	if( targetEle != 0 &&
		targetEle->GetText() != 0 &&
		strlen( targetEle->GetText() ) != 0 )
		targetName = targetEle->GetText();
}

void LookAt::Awake(void)
{
	// Find the first GameObject named targetName.
	GameObject* obj = GameObject::Find( targetName.c_str() );

	if( obj != 0 )
		targetId = obj->GetInstanceId();
}
