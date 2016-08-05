#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"

#include <list>

class Mover : public EDGameCore::Behavior<Mover>
{
public:
	void OnMessage(EDGameCore::Message& message);
	void OnMessage(EDGameCore::Message& message, const void * data);
};