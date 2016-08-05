#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"

class Picking : public EDGameCore::Behavior<Picking>
{
	EDGameCore::RayHit rayHit;
	Float3 localIntersection;
	Float3 newPosition;
	unsigned int manipulatedInstanceId;
	unsigned int selectedInstanceId;

public:
	Picking(void) : manipulatedInstanceId(0), selectedInstanceId(0) {}
	
	void OnMessage(EDGameCore::Message& message, const void *data);
	void OnMessage(EDGameCore::Message& message);

	void Update(void);
	void FixedUpdate(void);
};
