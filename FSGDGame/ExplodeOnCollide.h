#pragma once

#include "../EDGameCore/Behavior.h"

class ExplodeOnCollide : public EDGameCore::Behavior<ExplodeOnCollide>
{
	void OnCollisionEnter(EDGameCore::ICollider* thisCollider, EDGameCore::ICollider* otherCollider, const EDCollision::Contact& contact);
};