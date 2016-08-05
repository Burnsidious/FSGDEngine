#include "precompiled.h"

#include "../EDThreadPool/ThreadPool.h"
#include "../EDThreadPool/WorkerThread.h"
#include "../EDMemoryManager/MemoryManager.h"

#include "../EDUtilities/InternalOutput.h"

namespace EDThreadPool
{
	void ThreadPool::PostWorkSolution(WorkerThread* _worker_ptr)
	{
		if (_worker_ptr != nullptr)
		{
			queue_mutex.lock();
				workers.push(_worker_ptr);
			queue_mutex.unlock();

			work_condition.notify_one();
		}
	}

	void ThreadPool::GetWorkSolution(WorkerThread** _worker_ptr)
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		// condition variable will wait while predicate is false, in this case wait while
		// there is no work todo or run is still true
		work_condition.wait(lock,  [this]() { return (workers.empty() == false) || (run.load() == false); } );
	
		(*_worker_ptr) = nullptr;
		if (workers.empty() != true)
		{
			(*_worker_ptr) = workers.front();
			workers.pop();
		}
	}

	void ThreadPool::ExecuteSolution(void)
	{
		WorkerThread* worker = nullptr;
		static int thread_count = 0;

		// if we want to allocate on multiple threads we need to create a mini heap for each pool
		// need to calc the name in a slighly wacky manner since we can not dynamically allocate until the heap is ready
		char heap_name[256] = "Thread Pool Heap ";
		memset(heap_name, '\0', sizeof(char) * 256);
		heap_name[ std::strlen(heap_name) ] = '0' + thread_count++;

		EDMemoryManager::MemoryManager::GetInstance()->CreateHeap(MEGABYTE(1), heap_name);

		while(run.load())
		{
			// attempt to get work from que, will block if there is nothing to do
			GetWork(&worker);
		
			// if we found some work, do it
			if (worker != nullptr)
			{
				// complete worker's work and do callback
				worker->DoWork();
				worker->DoneCallback();
			}
		}

		EDMemoryManager::MemoryManager::GetInstance()->DestroyHeap();
	}

	void ThreadPool::InitSolution()
	{
		std::thread* thread = nullptr;
		unsigned int num_cpus = std::thread::hardware_concurrency();

		// hardware_concurrency returns a good "hint", can return 0 
		num_threads = (num_cpus > 0) ? num_cpus : 1;
		run.store(true);

		// create and initialize our threads and get them running, need to pass in this
		// since we are running a member function for threads
		for(unsigned int i = 0; i < num_threads; i++)
		{
			thread = new std::thread(&ThreadPool::Execute, this);
			threads.push_back(thread);
		}
	}

	void ThreadPool::ClearWorkSolution()
	{
		std::lock_guard<std::mutex> lockit(queue_mutex);
		while(workers.empty() == false)
		{
			workers.pop();
		}
	}

	void ThreadPool::ShutdownSolution()
	{
		// we're done, let the threads exit gracefully
		run.store(false);

		// wake everybody up
		work_condition.notify_all();

		// the sleeping threads should already be signaled, wait for the working threads to finish
		for(auto &t: threads)
		{
			t->join();
			delete t;
		}

		threads.clear();
		num_threads = 0;
	}

} // end of EDThreadPool namespace
