#pragma once

#include "../EDGameCore/Behavior.h"

class Crosshair : public EDGameCore::Behavior<Crosshair>
{
	void OnMessage(EDGameCore::Message& message);
};