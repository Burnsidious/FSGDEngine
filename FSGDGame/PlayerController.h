#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"

class PlayerController : public EDGameCore::Behavior<PlayerController>
{
public:
	PlayerController(void);

	void OnMessage(EDGameCore::Message& message);

	void Update(void);
	void FixedUpdate(void);
	void LateUpdate(void);

private:
	bool grounded;
	float transZ;
	float rotY;
	Float3 oldPos;
	bool jump;

	void Awake(void);
};