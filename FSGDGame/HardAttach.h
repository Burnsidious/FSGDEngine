#pragma once

#include "../EDGameCore/Behavior.h"

class HardAttach : public EDGameCore::Behavior<HardAttach>
{
	void LoadState(TiXmlElement* xmlElement);

	Float3 offset;
	std::string targetName;
	unsigned int targetId;
	bool clamp;

	void Update(void);
	void Awake(void);

public:
	unsigned int GetTargetId(void)const{ return targetId; }
};
