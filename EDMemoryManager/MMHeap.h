
#pragma once
#include <Windows.h>

#ifdef MM_SOLUTION_EXPORTS
#define MM_SOLUTION_API __declspec(dllexport)
#else
#define MM_SOLUTION_API __declspec(dllimport)
#endif

#define MM_HEAP_LOCK if(synchronize)EnterCriticalSection(&criticalSection)
#define MM_HEAP_UNLOCK if(synchronize)LeaveCriticalSection(&criticalSection)

namespace EDMemoryManager
{
	struct Header
	{
		// How much memory is free INSIDE the block : excludes header and footer size
		// High order bit of 1 == used memory block
		// High order bit of 0 == free memory block
		unsigned int size;
		// Pointer to the previous free header in the Cyclic Doubly Linked List
		Header * prev;
		// Pointer to the next free header in the Cyclic Doubly Linked List
		Header * next;
	};
	struct Footer
	{
		// How much memory is free INSIDE the block : excludes header and footer size
		// High order bit of 1 == used memory block
		// High order bit of 0 == free memory block
		unsigned int size;
	};

	class Heap
	{
		friend class MemoryManager;
		// Allocated memory for this entire heap
		char * pool;
		// Entry point to a cyclic doubly linked list of free memory blocks within the heap.  
		// This will be NULL if there is nothing in the list, otherwise it must always point to a free block.
		Header * freeListEntry;
		// First Header within the heap (due to padding, may not be the same address as mPool)
		Header * firstHeader;
		// Last Footer within the heap (due to padding, may not be 4 bytes before the end of the mPool array)
		Footer * lastFooter;
		// How much memory was allocated for mPool (not really used at this time).
		unsigned int poolSizeTotal;
		// If mSynchronize == 0, then there will not be any thread protection around allocate and deallocate, only set to zero if you are not multi-threading,
		// or only if each thread allocates and does it's own de-allocation (another thread will not free the memory allocated).
		unsigned int synchronize;
		CRITICAL_SECTION criticalSection;
public:
		MM_SOLUTION_API Header * FindBlockSolution(unsigned int allocSize);

		MM_SOLUTION_API bool Heap::AllocateSplitSolution(Header* _head, void** _data, int _alloc_size);
		MM_SOLUTION_API void Heap::AllocateNoSplitSolution(Header* _head, void** _data);
		MM_SOLUTION_API bool DeAllocateRightSolution(void * data);
		MM_SOLUTION_API bool DeAllocateLeftSolution(void * data);
		MM_SOLUTION_API void DeAllocateMiddleSolution(void * data);
		MM_SOLUTION_API void InitSolution(unsigned int poolSize, unsigned int _synchronize = 0);
		MM_SOLUTION_API void CoreDumpSolution();
		
		inline unsigned int GetPoolSizeTotal()	{return poolSizeTotal;}
		void Destroy();
		~Heap();

// STUDENT TODOS
public:
		Header * FindBlock(unsigned int allocSize);
		void Init(unsigned int poolSize, unsigned int _synchronize = 0);
		void * Allocate(unsigned int allocSize);
		void DeAllocate(void * data);
		void CoreDump();
	};
}