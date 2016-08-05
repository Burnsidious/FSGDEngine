#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"
#include <list>

class BuggyDriver : public EDGameCore::Behavior<BuggyDriver>
{
public:
	BuggyDriver(void);

	void OnMessage(EDGameCore::Message& message);
	void OnMessage(EDGameCore::Message& message, const void *data);

	void Update(void);
	void FixedUpdate(void);

private:

	float transZ;
	float rotY;
	float steeringAngle;

	float translationSpeed;
	float rotationSpeed;

	int gunMoved;
	//unsigned int targetID;

	struct bullet
	{
		Float3 pos;
		Float3 oldPos;
		float age;
	};

	static const unsigned int BULLET_MAX = 1000;
	bullet bullets[BULLET_MAX];

	static const float BULLET_FIRE_INTERVAL;
	float lastFiredTime;

	list<bullet*> liveBullets;
	list<bullet*> bulletCache;
};