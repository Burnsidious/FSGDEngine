#pragma once

#include "MemoryManager.h"

#ifdef new
//#undef new
#endif

inline void * operator new (size_t size)
{
	return EDMemoryManager::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
}

inline void * operator new (size_t size, const char *file, int line)
{
	return EDMemoryManager::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), 0U, file, line);
}

inline void operator delete (void * p)
{
	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(p);
}

// Unused
inline void operator delete (void * p, const char *file, int line)
{
	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(p);
}

inline void * operator new[] (size_t size, const char *file, int line)
{
	return EDMemoryManager::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), 0U, file, line);
}

inline void * operator new[] (size_t size)
{
	return EDMemoryManager::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
}

inline void operator delete[] (void * p)
{
	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(p);
}

// Unused
inline void operator delete[] (void * p, const char *file, int line)
{
	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(p);
}

#if MM_LOG_MEMORY_LEAKS
#define new new( __FILE__, __LINE__)
//#define delete delete( __FILE__, __LINE__) // We can't have nice things.
#endif
 