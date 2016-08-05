#include "Particle.h"
#include "RigidBody.h"

namespace EDPhysics
{
	void Update(Float3* pos, Float3* prev, unsigned int count, const Float3* accelerations)
	{
		for(unsigned int i = 0; i < count; ++i)
		{
			Float3& position = pos[i];
			Float3& previousPosition = prev[i];

			Float3 prevAccel = accelerations[i] * TIME_STEP_SQ;

			Float3 newPosition = position*1.999f - previousPosition*0.999f + prevAccel;

			previousPosition = position;
			position = newPosition;
		}
	}
}