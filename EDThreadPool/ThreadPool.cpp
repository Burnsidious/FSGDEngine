#include "precompiled.h"
#include "ThreadPool.h"

// these dependencies can be whacked when test objects are taken out for real tasks
#include "WorkerExample.h"
#include "../EDUtilities/InternalOutput.h"

namespace EDThreadPool
{
	ThreadPool* ThreadPool::GetInstance()
	{
		static ThreadPool thread_pool;
		return &thread_pool;
	}

	ThreadPool::~ThreadPool()
	{
	}

	ThreadPool::ThreadPool()
	{
		Init();
	}

	void ThreadPool::Init()
	{
		// TODO - implement your own version
		InitSolution();
	}

	void ThreadPool::Shutdown()
	{
		ClearWork();

		// TODO - implement your own version
		ShutdownSolution();

		// if any test objects were created clear them out before we quit
		for(auto &t : testObjects)
		{
			delete t;
		}
		testObjects.clear();
	}

	void ThreadPool::Execute(void)
	{
		// TODO - implement your own version
		ExecuteSolution();
	}

	void ThreadPool::PostWork(WorkerThread* _worker_ptr)
	{
		// TODO - implement your own version
		PostWorkSolution(_worker_ptr);
	}

	void ThreadPool::GetWork(WorkerThread** _worker_ptr)
	{
		// TODO - implement your own version
		GetWorkSolution(_worker_ptr);
	}

	void ThreadPool::ClearWork()
	{
		// TODO - implement your own version
		ClearWorkSolution();
	}

} // end of EDThreadPool namespace
