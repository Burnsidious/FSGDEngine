#pragma once

class EDTreeLeafNodeData;
class EDTreeNode
{
	friend class EDTree;
private:
	EDTreeNode(const EDTreeNode&){}
	EDTreeNode& operator=(const EDTreeNode&){}
	void EnumerateNodes(unsigned int& count);

protected:
	EDTreeNode* parentPtr;
	EDTreeNode* nextSiblingPtr;
	EDTreeNode* prevSiblingPtr;
	EDTreeNode* firstChildPtr;
	EDTreeNode* lastChildPtr;
	unsigned int nodeNumber;
public:
	EDTreeNode(void) : parentPtr(0), nextSiblingPtr(0), prevSiblingPtr(0), firstChildPtr(0), lastChildPtr(0), nodeNumber(0) {}
	virtual ~EDTreeNode(void)
	{
		delete firstChildPtr;
		delete nextSiblingPtr;
	}

	void SetParent(EDTreeNode *_parentPtr);
	virtual bool IsLeaf(void) const = 0;
	const EDTreeNode* GetParent(void) const { return parentPtr; }
	const EDTreeNode* GetNextSibling(void) const { return nextSiblingPtr; }
	const EDTreeNode* GetChildren(void) const { return firstChildPtr; }
	virtual void Traverse( void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, void* traversalDataPtr, void* traversalFuncResultPtr ) ) const = 0;
	void TraverseDFPost( void* traversalDataPtr, void (*TraversalFunc)(EDTreeNode*, void* traversalDataPtr) );
	unsigned int GetNodeNumber(void)const{ return nodeNumber; }
	void SetNodeNumber(unsigned int _nodeNumber){ nodeNumber = _nodeNumber; }
};

class EDTreeInternalNodeData
{
public:
	virtual ~EDTreeInternalNodeData(){}
};

class EDTreeInternalNode : public EDTreeNode
{
public:
	virtual ~EDTreeInternalNode(){}
	bool IsLeaf(void) const { return false; }
	virtual const EDTreeInternalNodeData* GetInternalNodeData(void) const = 0;
	virtual void SetInternalNodeData(EDTreeInternalNodeData* dataPtr) = 0;
};

class EDTree;
class EDTreeLeafNode;
class EDTreeLeafNodeData
{
public:
	virtual ~EDTreeLeafNodeData(){}
	virtual bool SplitData(EDTreeInternalNodeData*& splitter, EDTreeLeafNodeData*& resultPtr, unsigned int& newDataCount) = 0;
	virtual EDTreeLeafNode* CreateLeafNode(EDTree* ownerTreePtr) = 0;
	virtual EDTreeLeafNode* GetContainingLeaf(void) const = 0;
	virtual int GetLeafNumber(void) const = 0;
	virtual void SetLeafNumber(int number) = 0;
};

class EDTreeLeafNode : public EDTreeNode
{
public:
	virtual ~EDTreeLeafNode(){}
	bool IsLeaf(void) const { return true; }
	EDTreeNode* Split(void);
	virtual EDTreeInternalNode* CreateInternalNode(void) const = 0;
	virtual EDTreeLeafNodeData* GetLeafNodeData(void) const = 0;
	virtual void SetLeafNodeData(EDTreeLeafNodeData* dataPtr) = 0;
	virtual EDTree* GetOwnerTree(void) = 0;
	virtual void SetOwnerTree(EDTree* treePtr) = 0;
};
