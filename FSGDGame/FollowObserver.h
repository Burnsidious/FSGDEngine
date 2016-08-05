#pragma once

#include "../EDGameCore/Behavior.h"

class FollowObserver : public EDGameCore::Behavior<FollowObserver>
{
public:
	FollowObserver(void) : targetId(0) {}

	void SetTargetByTag(const char* tag);

private:
	string targetTag;
	unsigned int targetId;

	void LoadState(TiXmlElement* xmlElement);

	void Update(void);
};