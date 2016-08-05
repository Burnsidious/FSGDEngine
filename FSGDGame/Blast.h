#pragma once

#include "../EDGameCore/Behavior.h"

class Blast : public EDGameCore::Behavior<Blast>
{
	void Update(void);
	void OnTriggerStay(EDGameCore::ICollider* thisCollider, EDGameCore::ICollider* otherCollider, const EDCollision::Contact& contact);
	void Awake(void);

	float age;

public:
	Blast(void) : age(0.0f) {}

};