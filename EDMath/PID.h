#ifndef _PID_H_
#define _PID_H_

// A PID controller is a feedback control mechanism used to reduce the error between a current
// state and a desired state.  When working with a PID the constants must be tuned to behave
// properly, start with proportional (Kp), then introduce the remaining constants (Ki, Kd)

namespace EDMath
{
	class PID
	{
	private:
		// how much to dampen integral over time
		const float INTEGRAL_DAMPEN;
		
		// current error, Kp
		float proportional;

		// historic error, Ki
		float integral;

		// error changing over time, Kd
		float differential;

		float previous_error;
		float integral_error;

		float UpdateSolution(float _time_delta, float _current, float _target);

	public:

		PID();

		// calculates output based on time slice, where we are, and where we want to be
		// IN _timeDelta - the time slice of the current frame, used for integral and differential
		// IN _current - the current state of the data
		// IN _target - the desired state fo the data
		// RETURN - results of the pid, what we should add to the current data to get to the target
		float Update(float _time_delta, float _current, float _target);

		// sets the weights for the pid controller
		// IN _p - proportional constant (kp)
		// IN _i - integral constant (ki)
		// IN _d - differential constant (kd)
		void SetWeights(float _p, float _i, float _d);

		// clears the previous and historic error, used in case a something changes targets or states
		void ClearError();
	};
};

#endif
