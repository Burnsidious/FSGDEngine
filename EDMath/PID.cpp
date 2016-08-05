#include "PID.h"

namespace EDMath
{
	PID::PID() : INTEGRAL_DAMPEN(0.75f)
	{
		proportional = 0;
		integral = 0;
		differential = 0;
		previous_error = 0;
		integral_error = 0;
	}

	float PID::Update(float _time_delta, float _current, float _target)
	{
		float error = 0.0;
		float differential_error = 0.0f;
		float result = 0.0f;

		// protect against zero and close to zero startup issues
		if (_time_delta > 0.0001f)
		{
			result = UpdateSolution(_time_delta, _current, _target);
		}

		return result;
	}
	
	void PID::SetWeights(float _p, float _i, float _d)
	{
		proportional = _p;
		integral = _i;
		differential = _d;
	}

	void PID::ClearError()
	{
		previous_error = 0;
		integral_error = 0;
	}
};
