#pragma once
//#include "MemoryManager.h"
#include <memory>

// TEMPLATE CLASS MMMallocAllocator
template<class _Ty>
	class MMMallocAllocator
		: public std::_Allocator_base<_Ty>
	{	// generic MMMallocAllocator for objects of class _Ty
public:
	typedef std::_Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const value_type* const_pointer;
	typedef const value_type& const_reference;

	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template<class _Other>
		struct rebind
		{	// convert an MMMallocAllocator<_Ty> to an MMMallocAllocator <_Other>
		typedef MMMallocAllocator<_Other> other;
		};

	pointer address(reference _Val) const
		{	// return address of mutable _Val
		return (&_Val);
		}

	const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
		return (&_Val);
		}

	MMMallocAllocator() _THROW0()
		{	// construct default MMMallocAllocator (do nothing)
		}

	MMMallocAllocator(const MMMallocAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		MMMallocAllocator(const MMMallocAllocator<_Other>&) _THROW0()
		{	// construct from a related MMMallocAllocator (do nothing)
		}

	template<class _Other>
		MMMallocAllocator<_Ty>& operator=(const MMMallocAllocator<_Other>&)
		{	// assign from a related MMMallocAllocator (do nothing)
		return (*this);
		}

	void deallocate(pointer _Ptr, size_type)
		{	// deallocate object at _Ptr, ignore size
#if USE_MEMORY_MANAGER
			//EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(_Ptr);
			free(_Ptr);
#else
			//::operator delete(_Ptr);
			free(_Ptr);
#endif
		}

	pointer allocate(size_type _Count)
		{	// allocate array of _Count elements
#if USE_MEMORY_MANAGER
		//return (_Ty *)EDMemoryManager::MemoryManager::GetInstance()->Allocate(_Count*sizeof(_Ty),false,0,__FILE__,__LINE__);
		return (_Ty *)malloc(_Count*sizeof(_Ty);
#else
		//return (_Allocate(_Count, (pointer)0));
			return (_Ty *)malloc(_Count*sizeof(_Ty));
#endif
		}

	pointer allocate(size_type _Count, const void *)
		{	// allocate array of _Count elements, ignore hint
#if USE_MEMORY_MANAGER
		//return (_Ty *)EDMemoryManager::MemoryManager::GetInstance()->Allocate(_Count*sizeof(_Ty),false,0,__FILE__,__LINE__);
		return (_Ty *)malloc(_Count*sizeof(_Ty);
#else
			//return (allocate(_Count));
			return (_Ty *)malloc(_Count*sizeof(_Ty));
#endif
		}

	void construct(pointer _Ptr, const _Ty& _Val)
		{	// construct object at _Ptr with value _Val
			std::_Construct(_Ptr, _Val);
		}

	void destroy(pointer _Ptr)
		{	// destroy object at _Ptr
			std::_Destroy(_Ptr);
		}

	size_t max_size() const _THROW0()
		{	// estimate maximum array size
		size_t _Count = (size_t)(-1) / sizeof (_Ty);
		return (0 < _Count ? _Count : 1);
		}
	};

		// MMMallocAllocator TEMPLATE OPERATORS
template<class _Ty,
	class _Other> inline
	bool operator==(const MMMallocAllocator<_Ty>&, const MMMallocAllocator<_Other>&) _THROW0()
	{	// test for MMMallocAllocator equality (always true)
	return (true);
	}

template<class _Ty,
	class _Other> inline
	bool operator!=(const MMMallocAllocator<_Ty>&, const MMMallocAllocator<_Other>&) _THROW0()
	{	// test for MMMallocAllocator inequality (always false)
	return (false);
	}




	// CLASS MMMallocAllocator<void>
template<> class _CRTIMP2_PURE MMMallocAllocator<void>
	{	// generic MMMallocAllocator for type void
public:
	typedef void _Ty;
	typedef _Ty *pointer;
	typedef const _Ty *const_pointer;
	typedef _Ty value_type;

	template<class _Other>
		struct rebind
		{	// convert an MMMallocAllocator<void> to an MMMallocAllocator <_Other>
		typedef MMMallocAllocator<_Other> other;
		};

	MMMallocAllocator() _THROW0()
		{	// construct default MMMallocAllocator (do nothing)
		}

	MMMallocAllocator(const MMMallocAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		MMMallocAllocator(const MMMallocAllocator<_Other>&) _THROW0()
		{	// construct from related MMMallocAllocator (do nothing)
		}

	template<class _Other>
		MMMallocAllocator<_Ty>& operator=(const MMMallocAllocator<_Other>&)
		{	// assign from a related MMMallocAllocator (do nothing)
		return (*this);
		}
	};

