
#include "HeapLog.h"
#include <iomanip>
#include <fstream>
#include <filesystem>

namespace EDMemoryManager
{
	HeapLog::HeapLog()
	{
		numberOfAllocations = 0;
		totalSizeAllocated = 0;
		largest_allocation = 0;
		smallest_allocation = INT_MAX;

		InitializeCriticalSection(&criticalSection);
	}

	HeapLog::~HeapLog()
	{
		DeleteCriticalSection(&criticalSection);
	}

	void HeapLog::Add(void * block, MemBlockLogInfo & info)
	{
		if(block == nullptr)
		{
			return;
		}

		EnterCriticalSection(&criticalSection);

			alloc_sizes[info.size] = alloc_sizes[info.size] + 1;
			allocatedBlocks[block] = info;
			totalSizeAllocated += info.size;
			numberOfAllocations++;

			if ((unsigned int)info.size > largest_allocation)
				largest_allocation = info.size;
			else if ((unsigned int)info.size < smallest_allocation)
				smallest_allocation = info.size;

		LeaveCriticalSection(&criticalSection);
	}
	void HeapLog::Remove(void * block)
	{
		if(block == nullptr)
		{
			return;
		}

		EnterCriticalSection(&criticalSection);

			totalSizeAllocated -= allocatedBlocks[block].size;
			allocatedBlocks.erase(block);
			numberOfAllocations--;

		LeaveCriticalSection(&criticalSection);
	}

	void HeapLog::WriteMetricFile(const char * heapName)
	{
		std::string file_name("../Logs/MemMetrics.txt");
		file_name.insert( file_name.length() - 4, " - ");
		file_name.insert( file_name.length() - 4, heapName);
		std::ofstream file(file_name.c_str());

		EnterCriticalSection(&criticalSection);
			if (file.is_open())
			{
				file << "Writing metrics for heap - " << heapName;
				file << "\n\nNumber of Allocations\t\t" << numberOfAllocations;
				file << "\nTotal Allocated\t\t\t" << totalSizeAllocated;
				file << "\nSmallest Allocated\t\t" << smallest_allocation;
				file << "\nLargest Allocated\t\t" << largest_allocation;
				file << "\nAverage Allocation\t\t" << (totalSizeAllocated / ((numberOfAllocations) ? numberOfAllocations : 1));
				file << "\nPercenet Used\t\t\t%" << ((float)totalSizeAllocated / 16777216.0f * 100.0f);
				file << "\n\nAlloc Size\t\tNum Allocations\n";
				for(auto& iter : alloc_sizes)
				{
					file << iter.first << "\t\t\t" <<  iter.second << "\n";
				}

				file.close();
			}
		EnterCriticalSection(&criticalSection);
	}

	void HeapLog::WriteLogFile(const char * heapName)
	{
		std::string file_name(LEAK_LOG_LOCATION);
		file_name.insert( file_name.length() - 4, " - ");
		file_name.insert( file_name.length() - 4, heapName);

		std::tr2::sys::path leakLogPath = std::tr2::sys::path(LEAK_LOG_LOCATION);
		if(std::tr2::sys::exists(leakLogPath))
		{
			std::tr2::sys::remove(leakLogPath);
		}

		if(allocatedBlocks.empty())
		{
			return;
		}
		
		if(leakLogPath.has_parent_path() && !std::tr2::sys::exists(leakLogPath.parent_path()))
		{
			std::tr2::sys::create_directories(leakLogPath.parent_path());
		}

		//std::fstream file(LEAK_LOG_LOCATION, std::ios_base::out);
		std::fstream file(file_name.c_str(), std::ios_base::out);
		if (!file.is_open())
		{
			file.close();
			file.clear();
			//file.open(LEAK_LOG_LOCATION, std::ios_base::trunc | std::ios_base::out);
			file.open(file_name.c_str(), std::ios_base::trunc | std::ios_base::out);
		}
		if (file.is_open())
		{
			file << "********************************************\n";
			file << "HeapName:\t" << heapName << '\n';
			file << "NumLeaks:\t" << allocatedBlocks.size() << '\n';
			file << "********************************************\n";
			file << "Address                Size    File\n";

			for (AllocatedBlocksType::iterator iter = allocatedBlocks.begin(); iter != allocatedBlocks.end(); ++iter)
			{
				file << "0x" << std::hex << std::setw(16) << std::setfill('0') << (*iter).first;
				file << " " << std::dec << std::setw(8) << std::setfill(' ') << (*iter).second.size << "    ";

				if((*iter).second.fileName[0] != '\0')
				{
					file << (*iter).second.fileName << ":" << (*iter).second.lineNum;
				}

				file << '\n';
			}

			file.close();
			file.clear();
		}
	}
}