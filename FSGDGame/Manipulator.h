#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"

class Manipulator : public EDGameCore::Behavior<Manipulator>
{
	unsigned int manipulatedInstanceId;
	unsigned int selectedInstanceId;

	Float3 localIntersection;
	Float3 newPosition;
	bool dragEnabled;
	EDGameCore::RayHit rayHit;

public:
	Manipulator(void) : manipulatedInstanceId(0), selectedInstanceId(0) {}

	void OnMessage(EDGameCore::Message& message, const void *data);
	void OnMessage(EDGameCore::Message& message);

	void Update(void);
	void FixedUpdate(void);
};