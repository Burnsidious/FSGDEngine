#include "../EDMath/PID.h"
#include "../EDUtilities/InternalOutput.h"

namespace EDMath
{
	float PID::UpdateSolution(float _time_delta, float _current, float _target)
	{
		float error = 0.0;
		float differential_error = 0.0f;
		float result = 0.0f;

		// proportional, what is the error currently
		error = _target - _current;

		// integral, what is the historic error
		integral_error = INTEGRAL_DAMPEN * integral_error + _time_delta * error;

		// differential, how is the error changing
		differential_error = (error - previous_error) / _time_delta;
		previous_error = error;

		// sum final result:  Kp * current error + Ki * historic error + Kd * changing error
		result = (proportional * error) + (integral * integral_error) + (differential * differential_error);

		return result;
	}
};
