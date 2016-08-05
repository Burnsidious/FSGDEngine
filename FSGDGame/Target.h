#pragma once

#include "../EDGameCore/Behavior.h"

class Target : public EDGameCore::Behavior<Target>
{
public:
	Target(void) : spinTimer(0.0f) {}

private:
	void Update(void);
	void OnMessage(EDGameCore::Message& message);
	float spinTimer;
};