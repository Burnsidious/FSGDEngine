#pragma once

#include "../EDGameCore/Behavior.h"

class JumpPad : public EDGameCore::Behavior<JumpPad>
{
public:
	void Update(void);

	void OnTriggerExit(EDGameCore::ICollider* thisCollider, EDGameCore::ICollider* otherCollider, const EDCollision::Contact& contact);
};