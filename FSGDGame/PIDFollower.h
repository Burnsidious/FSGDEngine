#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"
#include "../EDGameCore/GameObject.h"
#include "../EDMath/PID.h"

#include <list>

using namespace EDMath;

class PIDFollower : public EDGameCore::Behavior<PIDFollower>
{
private:

	void PIDFollowerSolution();
	void Awake(void);

	std::string targetName;
	unsigned int targetId;
	EDGameCore::GameObject* target;
	
	PID x_pid;
	PID y_pid;
	PID z_pid;

	float spin;
	float bounce;

public:

	PIDFollower();

	void LateUpdate();
	void LoadState(TiXmlElement* xmlElement);


};
