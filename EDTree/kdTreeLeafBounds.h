//#pragma once
//
//#include "kdTree.h"
//#include "../EDCollision/CollisionLibrary.h"

//class kdTreeLeafBounds : public EDTreeLeafNodeData
//{
//	EDCollision::Aabb aabb;
//	EDTreeLeafNode* containingLeafPtr;
//
//	#if MM_AVAILABLE
//	std::list<void*, MMAllocator<void*>> containedItemsPtr; 
//	#else
//	std::list<void*> containedItemsPtr;	
//	#endif
//
//public:
//	kdTreeLeafBounds(void) : containingLeafPtr(0) {}
//	virtual ~kdTreeLeafBounds(void){}
//
//	kdTreeLeafBounds(const EDCollision::Aabb &_aabb)
//	{ 
//		aabb = _aabb;
//		containingLeafPtr = 0;
//	}
//
//	kdTreeLeafBounds(const Float3& vmin, const Float3& vmax)
//	{ 
//		containingLeafPtr = 0;
//		aabb.min = vmin; 
//		aabb.max = vmax;
//	}
//
//	bool SplitData(EDTreeInternalNodeData*& splitter, EDTreeLeafNodeData*& resultPtr, unsigned int& newDataCount);
//	EDTreeLeafNode* CreateLeafNode(EDTree *ownerTreePtr);
//	EDTreeLeafNode* GetContainingLeaf(void) const;
//
//	void AddContainedItem(void* itemPtr){ containedItemsPtr.push_back( itemPtr ); }
//	void RemoveContainedItem(void* itemPtr)
//	{
//		#if MM_AVAILABLE
//		std::list<void*, MMAllocator<void*>>::iterator iter = containedItemsPtr.begin();
//		#else
//		std::list<void*>::iterator iter = containedItemsPtr.begin();
//		#endif
//
//		for(; iter != containedItemsPtr.end(); ++iter )
//		{
//			if( (*iter) == itemPtr )
//			{
//				containedItemsPtr.erase( iter );
//				break;
//			}
//		}
//	}
//};