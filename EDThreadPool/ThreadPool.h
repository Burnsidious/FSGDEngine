#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

/*  The ThreadPool class is designed to accept work tasks and assign the work to idle threads.
*	Subclass the WorkerThread to create workers
*/

namespace EDThreadPool
{
	class WorkerThread;

	class ThreadPool
	{
	private:

		// --- attributes ---

		// the number of spawned threads
		unsigned int num_threads;
	
		// a lock to protect the work que
		std::mutex queue_mutex;

		// sleep threads based queue size and run atomic
		std::condition_variable work_condition;

		// a list of worker threads
		std::queue<WorkerThread*> workers;

		// storage for threads
		std::vector<std::thread*> threads;

		// keep track if the threadpool needs to run
		std::atomic_bool run;
	
		// --- methods ---

		void InitSolution();
		void ShutdownSolution();
		void PostWorkSolution(WorkerThread* _worker_ptr);
		void GetWorkSolution(WorkerThread** _worker_ptr);
		void ExecuteSolution();
		void ClearWorkSolution();

		ThreadPool();
		~ThreadPool();

		// initializes internal members and creates threads
		void Init();

		// the work horse function for all threads to execute
		void Execute(void);

	public:

		// singleton interface
		static ThreadPool* GetInstance();

		// increases the work count, adds work to the que and posts the work
		// IN _worker_ptr - work to be added to queue
		void PostWork(WorkerThread* _worker_ptr);

		// waits for work to be posted, grabs a work item from the que
		// OUT _worker_ptr - pointer to mem address for returned work pointer
		void GetWork(WorkerThread** _worker_ptr);

		// removes all items from the work queue
		void ClearWork();

		// cleans up allocated resources
		void Shutdown();

		// temporary storage for clean up
		std::list<WorkerThread*> testObjects;
	};

} // end of EDThreadPool namespace

#endif
