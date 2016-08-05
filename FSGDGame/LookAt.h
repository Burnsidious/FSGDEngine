#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"
#include "../EDGameCore/GameObject.h"

#include <list>

class LookAt : public EDGameCore::Behavior<LookAt>
{
public:

	LookAt();

	void LateUpdate();
	void LoadState(TiXmlElement* xmlElement);

private:

	void LookAtSolution();

	std::string targetName;
	unsigned int targetId;
	EDGameCore::GameObject* target;

	void Awake(void);
};
