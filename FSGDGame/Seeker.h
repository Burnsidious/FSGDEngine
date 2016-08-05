#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"

class Seeker : public EDGameCore::Behavior<Seeker>
{
public:
	Seeker(void);

	void Update(void);

private:
	
	unsigned int targetId;
	//unsigned int targetMask;
	unsigned int targetTag;
	std::string trackingJoint;

	//float searchRadius;

	void LoadState(TiXmlElement* xmlElement);

	void OnTriggerStay(EDGameCore::ICollider* thisCollider, EDGameCore::ICollider* otherCollider, const EDCollision::Contact& contact);
};