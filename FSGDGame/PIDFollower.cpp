#include <list>
#include "PIDFollower.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/RigidBody.h"
#include "../EDGameCore/Collider.h"
#include "../EDGameCore/Game.h"
#include "../EDGameCore/GameObject.h"

using namespace std;
using namespace EDGameCore;

PIDFollower::PIDFollower()
{
	target = nullptr;
	targetId = 0;
	spin = 0.0f;
	bounce = 5.0f;
}

void PIDFollower::LateUpdate()
{
	Transform* looker_transform = GetGameObject()->GetTransform();
	Transform* target_transform = nullptr;
	float delta_time = EDGameCore::Game::GetDeltaTime();

	// Finds the target based on its instance id each update 
	// to avoid having a dangling pointer to an object that might have been destroyed.
	target = GameObject::GetGameObjectInstance(targetId);

	if (target != nullptr)
	{
		// TODO - comment this out and write your own solution
		PIDFollowerSolution();
	}
}

void PIDFollower::LoadState(TiXmlElement* xmlElement)
{
	string p, i, d;
	float fp, fi, fd;
	// Get the name of the target to look-at
	TiXmlElement* targetEle = xmlElement->FirstChildElement("TargetName");
	if( targetEle != 0 &&
		targetEle->GetText() != 0 &&
		strlen( targetEle->GetText() ) != 0 )
		targetName = targetEle->GetText();

	targetEle = xmlElement->FirstChildElement("Proportional");
	if (targetEle != 0 &&
		targetEle->GetText() != 0 &&
		strlen(targetEle->GetText()) != 0)
		p = targetEle->GetText();

	targetEle = xmlElement->FirstChildElement("Integral");
	if (targetEle != 0 &&
		targetEle->GetText() != 0 &&
		strlen(targetEle->GetText()) != 0)
		i = targetEle->GetText();

	targetEle = xmlElement->FirstChildElement("Differential");
	if (targetEle != 0 &&
		targetEle->GetText() != 0 &&
		strlen(targetEle->GetText()) != 0)
		d = targetEle->GetText();

	fp = stof(p);
	fi = stof(i);
	fd = stof(d);

	x_pid.SetWeights(fp, fi, fd);
	y_pid.SetWeights(fp, fi, fd);
	z_pid.SetWeights(fp, fi, fd);
}

void PIDFollower::Awake(void)
{
	// Find the first GameObject named targetName.
	GameObject* obj = GameObject::Find( targetName.c_str() );

	if( obj != 0 )
		targetId = obj->GetInstanceId();
}
