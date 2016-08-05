#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"
#include "../EDGameCore/GameObject.h"

#include <list>

#define DEAD_ZONE 0.001f

class TurnTo : public EDGameCore::Behavior<TurnTo>
{
public:

	TurnTo();

	void LateUpdate();
	void LoadState(TiXmlElement* xmlElement);

private:

	void TurnToSolution();
	void Awake(void);

	std::string targetName;
	unsigned int targetId;
	float turn_speed;
	EDGameCore::GameObject* target;
};
