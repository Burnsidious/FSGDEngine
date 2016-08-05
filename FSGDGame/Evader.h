#pragma once

#include "../EDGameCore/Behavior.h"

class Evader : public EDGameCore::Behavior<Evader>
{
public:
	Evader(void);
	
	void Update(void);

private:
	
	unsigned int evadingId;
	unsigned int evadingTag;

	void OnTriggerStay(EDGameCore::ICollider* thisCollider, EDGameCore::ICollider* otherCollider, const EDCollision::Contact& contact);

	void LoadState(TiXmlElement* xmlElement);
};