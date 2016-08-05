#include <fstream>
#include <chrono>
#include <iomanip>

#include "MMHeap.h"

namespace EDMemoryManager
{	
	Header * Heap::FindBlock(unsigned int allocSize)
	{
		return FindBlockSolution(allocSize);
	}

	void Heap::Init(unsigned int poolSize, unsigned int _synchronize)
	{
		//synchronize = _synchronize;
		//InitializeCriticalSection(&criticalSection);
		//MM_HEAP_LOCK;

		// TODO - comment out the solution version and write your own, you can ignore the synchronization code
		InitSolution(poolSize, _synchronize);
	
		//MM_HEAP_UNLOCK;
	}

	void* Heap::Allocate(unsigned int allocSize)
	{
		// MM_HEAP_LOCK;

		Header* free_head = FindBlockSolution(allocSize);
		Footer* free_foot = nullptr;
		Header* used_head = nullptr;
		Footer* used_foot = nullptr;
		void* data = nullptr;

		// TODO - comment out the solution version and write your own, you can ignore the synchronization code
		if (AllocateSplitSolution(free_head, &data, allocSize) == false)
		{
			AllocateNoSplitSolution(free_head, &data);
		}

		// MM_HEAP_UNLOCK;
		return data;
	}

	void Heap::DeAllocate(void * data)
	{
		// MM_HEAP_LOCK;
		bool merged_left = false;
		bool merged_right = false;

		// TODO - comment out the solution version and write your own, you can ignore the synchronization code
		merged_right = DeAllocateRightSolution(data);
		merged_left = DeAllocateLeftSolution(data);
		
		if (merged_left != true)
		{
			DeAllocateMiddleSolution(data);
		}

		// MM_HEAP_UNLOCK;
	}
	
	void Heap::CoreDump()
	{
		// MM_HEAP_LOCK;

		// TODO - comment out the solution version and write your own, you can ignore the synchronization code
		CoreDumpSolution();

		// kick start
		//std::ofstream file("..\\Logs\\coredump.txt", std::ios_base::out | std::ios_base::trunc);
		//if (file.is_open())
		//{
		//	std::time_t time = std::time(nullptr);
		//	std::tm local_time = *std::localtime(&time);
		//	file << "Core Dump on " << std::put_time(&local_time, "%m-%d-%Y %I:%M") << "\n\n";
		//	file << "Mem Address\tData";
		//	file << std::hex;

		//	//some sort of looping mechanism, until it works just print out a few hundred bytes or so
		//	//		print out the memory address - 0x000012ab
		//	//		print out data at the memory address chopped up into bytes		44 12 ab 6e
		//	//		print out the chopped up data interpeted as a character			0=a1
		//	//		0x000012ab		44 12 ab 6e			0=a1

		//	file.close();
		//}

		// MM_HEAP_UNLOCK;
	}
}
