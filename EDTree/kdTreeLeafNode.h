#pragma once

#include "EDTree.h"

#include "kdTreeInternalNode.h"
#include "../EDCollision/CollisionLibrary.h"

class kdTreeLeafNode : public EDTreeLeafNode
{
public:
	class kdTreeLeafBounds : public EDTreeLeafNodeData
	{
		EDCollision::Aabb aabb;
		kdTreeLeafNode* containingLeafPtr;
		int leafNumber;
		std::list< void* > containedItemsPtr;	
	public:
		kdTreeLeafBounds(void) : containingLeafPtr(0), leafNumber(-1) {}
		kdTreeLeafBounds(const EDCollision::Aabb &_aabb);
		kdTreeLeafBounds(const EDMath::Float3& vmin, const EDMath::Float3& vmax);
		virtual ~kdTreeLeafBounds(void){}
		bool SplitData(EDTreeInternalNodeData*& splitter, EDTreeLeafNodeData*& resultPtr, 
			unsigned int& newDataCount);
		kdTreeLeafNode* CreateLeafNode(EDTree *ownerTreePtr);
		kdTreeLeafNode* GetContainingLeaf(void) const;
		void AddContainedItem(void* itemPtr);
		void RemoveContainedItem(void* itemPtr);
		const EDCollision::Aabb& GetBounds(void)const { return aabb; }
		std::list< void* >& GetContainedItems(void){ return containedItemsPtr; }
		int GetLeafNumber(void)const{ return leafNumber; }
		void SetLeafNumber(int _leafNumber){ leafNumber = _leafNumber; }
	};

private:
	kdTreeLeafBounds* leafBoundsPtr;
	EDTree* ownerTreePtr;

public:
	kdTreeLeafNode(void) : leafBoundsPtr(0), ownerTreePtr(0) {}

	kdTreeInternalNode* CreateInternalNode(void) const
	{
		return new kdTreeInternalNode;
	}
	
	~kdTreeLeafNode()
	{
		delete leafBoundsPtr;
	}

	kdTreeLeafBounds* GetLeafNodeData(void) const
	{
		return leafBoundsPtr;
	}

	void SetLeafNodeData(EDTreeLeafNodeData* dataPtr)
	{
		leafBoundsPtr = (kdTreeLeafBounds*)dataPtr;
	}

	EDTree* GetOwnerTree(void){ return ownerTreePtr; }
	void SetOwnerTree(EDTree* treePtr){ ownerTreePtr = treePtr; }

	void Traverse( void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, 
		void* traversalDataPtr, void* traversalFuncResultPtr ) ) const;
};