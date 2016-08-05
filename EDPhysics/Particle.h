#pragma once

#include "EDPhysics.h"

namespace EDPhysics
{
	void Update(Float3* pos, Float3* prev, unsigned int count, const Float3* accelerations);
}