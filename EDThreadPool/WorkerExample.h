#ifndef _WORKER_EXAMPLE_H_
#define _WORKER_EXAMPLE_H_


#include "../EDUtilities/InternalOutput.h"
#include "WorkerThread.h"

/* an example class of how to subclass a WorkerThread and supply work to the thread pool
*/

namespace EDThreadPool
{
	class WorkerExample : public WorkerThread
	{
	public:
		int radix;
		int primes_found;

	public:
		WorkerExample(int _radix)
		{
			radix = _radix;
			primes_found = 0;
		}

		void DoneCallback()
		{
			EDUtilities::InternalOutput::GetReference() << "Thread Pool found " << primes_found << " primes between 1 and " << radix << "\n";
		}

		void DoWork()
		{
			for (int i = 2; i < radix; i++)
			{
				bool is_prime = true;
				for (int j = 2; j < (int)(radix / 2); j++)
				{
					// if the divisor has grown to size of int we are checking
					if (j >= i)
						break;

					// if it is divisible must not be prime
					if (i % j == 0)
					{
						is_prime = false;
						break;
					}
				}

				if (is_prime == true)
				{
					primes_found++;
				}
			}
		}
	};
}

#endif
