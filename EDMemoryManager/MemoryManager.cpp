#include "MemoryManager.h"
#include <assert.h>

namespace EDMemoryManager
{
	MemoryManager *MemoryManager::GetInstance()
	{
		static MemoryManager theInstance;
		return &theInstance;
	}

	MemoryManager::MemoryManager()
	{
		heaps = (HeapData *)malloc(sizeof(HeapData) * MM_MAX_NUM_HEAPS);

		collect_metrics = false;

#if _DEBUG
		collect_metrics = true;
#endif

//#if MM_LOG_MEMORY_LEAKS 
		for(int heapIndex = 0; heapIndex < MM_MAX_NUM_HEAPS; ++heapIndex)
		{
			// We must explicitly call the HeapLog constructor because the memory was malloc'd
			new (&heaps[heapIndex].log) HeapLog();
		}
//#endif
		InitializeCriticalSection(&criticalSection);
		CreateHeap(HEAP_SIZE_MB * 1024 * 1024, "Main Game Thread Heap");
	}

	MemoryManager::~MemoryManager()
	{
//#if MM_LOG_MEMORY_LEAKS 
		for(int heapIndex = 0; heapIndex < MM_MAX_NUM_HEAPS; ++heapIndex)
		{
			DestroyHeap(heapIndex);
		}
//#endif
		DeleteCriticalSection(&criticalSection);
	}

	unsigned int MemoryManager::CreateHeap(unsigned int heapSize, const char *heapName, unsigned int flags)
	{
		static unsigned int i = 0;
		unsigned int retval = -1;

		// create heap, initialize, grab thread id, increment internal counter for the next heap to be created
		EnterCriticalSection(&criticalSection);

			// internal counter has not passed the max number of allocated heaps
			if (i < MM_MAX_NUM_HEAPS)
			{
				heaps[i].heap.Init(heapSize, flags & MM_SYNCHRONIZE);

				if(heapName)
				{
					memcpy(heaps[i].heapName,heapName, strlen(heapName) + 1);
				}

				heaps[i].threadId = GetCurrentThreadId();
				retval = i;
				i++;
			}

		LeaveCriticalSection(&criticalSection);

		return retval;
	}

	void MemoryManager::DestroyHeap(unsigned int heapId)
	{
		EnterCriticalSection(&criticalSection);

		unsigned int threadId = GetCurrentThreadId();
		for (unsigned int i = 0; i < MM_MAX_NUM_HEAPS; ++i)
		{
			if (threadId == heaps[i].threadId)
			{
				heaps[i].log.WriteLogFile(heaps[i].heapName);

				// We need to explicitly call HeapLog destructor because the memory was malloc'd
				// TODO: Make sure this is actually destructing AllocatedBlocksType
				heaps[i].log.~HeapLog();

				// since this heap is being destroyed we can invalidate the thread id 
				heaps[i].threadId = -1;
				break;
			}
		}
		LeaveCriticalSection(&criticalSection);
	}

	void *MemoryManager::Allocate(unsigned int allocSize, unsigned int heapId, const char *fileName, int lineNum)
	{
		if (heapId)
		{
			if (heapId < MM_MAX_NUM_HEAPS)
			{
				void *ret = heaps[heapId].heap.Allocate(allocSize);

				if (collect_metrics)
				{
					MemBlockLogInfo info;
					if(fileName)
					{
						memcpy(info.fileName, fileName, strlen(fileName) + 1);
					}
					info.lineNum = lineNum;
					info.size = allocSize;
					heaps[heapId].log.Add(ret, info);
				}

				return ret;
			}
			// Heap Not Valid
			assert(false);
			return nullptr;
		}

		const unsigned int curThreadId = GetCurrentThreadId();
		for (unsigned int heapIndex = 0; heapIndex < MM_MAX_NUM_HEAPS; ++heapIndex)
		{
			if (curThreadId == heaps[heapIndex].threadId)
			{
				void *mem = heaps[heapIndex].heap.Allocate(allocSize);
				if (mem)
				{
					if (collect_metrics)
					{
						MemBlockLogInfo info;
						if(fileName)
						{
							memcpy(info.fileName,fileName, strlen(fileName) + 1);
						}
						else
						{
							info.fileName[0] = '\0';
						}
						info.lineNum = lineNum;
						info.size = allocSize;
						heaps[heapIndex].log.Add(mem, info);
					}
					return mem;
				}	
			}	
		}

		// No heap memory was available for this thread.
		//assert(false);
		return nullptr;
	}

	void MemoryManager::DeAllocate(void *data, unsigned int heapId)
	{
		if (!data)
		{
			return;
		}

		if (heapId)
		{
			if (heapId < MM_MAX_NUM_HEAPS)
			{
				if (collect_metrics)
				{
					heaps[heapId].log.Remove(data);
				}
				heaps[heapId].heap.DeAllocate(data);
				return;	
			}
			// Heap Not Valid
			assert(false);
		}

		int heapIndex = -1;
		for (int i = 0; i < MM_MAX_NUM_HEAPS; ++i)
		{
			if (data > heaps[i].heap.firstHeader && data < heaps[i].heap.lastFooter)
			{
				heapIndex = i;
				break;
			}	
		}

		if (collect_metrics)
		{
			heaps[heapIndex].log.Remove(data);
		}

		heaps[heapIndex].heap.DeAllocate(data);
	}

	size_t MemoryManager::GetTotalMemoryUsed(unsigned int heapId)
	{
		return heaps[heapId].log.GetTotalSizeAllocated();
	}

	size_t MemoryManager::GetTotalMemory(unsigned int heapId)
	{
		return heaps[heapId].heap.GetPoolSizeTotal();
	}

	unsigned int MemoryManager::GetNumberOfAllocations(unsigned int _heap_id)
	{
		return heaps[_heap_id].log.GetNumberOfAllocations();
	}

	unsigned int MemoryManager::GetTotalMemoryAvailable(unsigned int _heap_id)
	{
		return heaps[_heap_id].heap.GetPoolSizeTotal() - heaps[_heap_id].log.GetTotalSizeAllocated() - GetDescriptorSize(_heap_id);
	}

	unsigned int MemoryManager::GetSmallestAllocation(unsigned int _heap_id)
	{
		return heaps[_heap_id].log.GetSmallestAllocation();
	}

	unsigned int MemoryManager:: GetLargestAllocation(unsigned int _heap_id)
	{
		return heaps[_heap_id].log.GetLargestAllocation();
	}

	unsigned int MemoryManager::GetDescriptorSize(unsigned int _heap_id)
	{
		return (heaps[_heap_id].log.GetNumberOfAllocations() * 16);
	}

	unsigned int MemoryManager:: GetAverageAllocation(unsigned int _heap_id)
	{
		unsigned int num_allocs = heaps[_heap_id].log.GetNumberOfAllocations();
		return heaps[_heap_id].log.GetTotalSizeAllocated() / ((num_allocs) ? num_allocs : 1);
	}

	void MemoryManager::CoreDump(unsigned int _heap_id)
	{
		heaps[_heap_id].heap.CoreDump();
	}
	
	void MemoryManager::MetricDump(unsigned int _heap_id)
	{
		heaps[_heap_id].log.WriteMetricFile(heaps[_heap_id].heapName);
	}
}
