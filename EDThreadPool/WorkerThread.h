#ifndef _WORKER_THREAD_H_
#define _WORKER_THREAD_H_


namespace EDThreadPool
{
	/*	WorkerThread is an abstract base class the the user will need to sub 
	class in order to provide the thread pool with some work to do.
	*/

	class WorkerThread
	{
	public:

		virtual void DoWork() = 0;
		virtual void DoneCallback() {}
	};

	/*	If you do not want to subclass something on your own and you want to use function 
		pointers you can create an instance of this class
	*/

	class WorkerThread_FP : public WorkerThread
	{
	public:

		std::function< void() > function_pointer;
		std::function< void() > done_callback;

		virtual void DoWork()
		{
			if (function_pointer)
			{
				function_pointer();
			}
		}

		virtual void DoneCallback()
		{
			if (done_callback)
			{
				done_callback();
			}
		}
	};

} // end of EDThreadPool namespace

#endif
