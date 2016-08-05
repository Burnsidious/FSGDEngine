#pragma once

#include "EDTreeNode.h"


class EDTree
{
private:
	friend void CountAndCollect(EDTreeNode* nodePtr, void* traversalDataPtr)
	{
		EDTree* treePtr = (EDTree*)traversalDataPtr;
		if( nodePtr->IsLeaf() )
			treePtr->StoreLeaf((EDTreeLeafNode*)nodePtr);
		
		nodePtr->SetNodeNumber( treePtr->numNodes );
		treePtr->numNodes += 1;
	}

protected:
	EDTreeInternalNode* rootNodePtr;
	unsigned int numNodes;

	//std::list< EDTreeLeafNode* > leafListPtr;
	std::vector< EDTreeLeafNode* > leafListPtr;

	void BuildTree(EDTreeLeafNodeData* startDataPtr)
	{
		delete rootNodePtr;
		rootNodePtr = 0;
		numNodes = 0;

		EDTreeLeafNode* firstLeafPtr = startDataPtr->CreateLeafNode(this);
			
		EDTreeNode* resultPtr = firstLeafPtr->Split();

		if( resultPtr != firstLeafPtr )
		{
			rootNodePtr = (EDTreeInternalNode*)resultPtr;
			//rootNodePtr->EnumerateNodes(numNodes);
		}
		else
			delete firstLeafPtr;
	}
	
public:
	EDTree(void) : rootNodePtr(0), numNodes(0) {}

	virtual ~EDTree()
	{
		delete rootNodePtr;
	}

	void Traverse(void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, void* traversalDataPtr, void* traversalFuncResultPtr));
	void TraverseDFPost(void* traversalDataPtr, void (*TraversalFunc)(EDTreeNode*, void* traversalDataPtr));

	void StoreLeaf(EDTreeLeafNode* leafPtr)
	{
		leafPtr->GetLeafNodeData()->SetLeafNumber((int)leafListPtr.size());
		leafListPtr.push_back(leafPtr);
	}

	std::vector< EDTreeLeafNode* > & GetLeaves(void){ return leafListPtr; }

	unsigned int GetNumberNodes(void){ return numNodes; }
	void SetNumberNodes(unsigned int _numNodes){ numNodes = _numNodes; }

	void SetRootNode(EDTreeInternalNode* rootPtr)
	{
		delete rootNodePtr;
		rootNodePtr = rootPtr;
		leafListPtr.clear();
		numNodes = 0;

		if( rootPtr != 0 )
			TraverseDFPost( this, CountAndCollect );
	}

	const EDTreeInternalNode* GetRoot(void) const { return rootNodePtr; }
};
