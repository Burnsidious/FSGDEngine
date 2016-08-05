#pragma once

#include "AabbBvh.h"
#include "AabbBvhInternalNode.h"

class AabbBvhLeafNode : public EDTreeLeafNode
{
public:
	class AabbBvhLeafData : public EDTreeLeafNodeData
	{
		EDCollision::Aabb aabb;
		AabbBvhLeafNode* containingLeafPtr;

		unsigned int* triangleIndicesPtr;
		unsigned int numTriangleIndices;
		unsigned int* vertexIndicesPtr;
		int leafNumber;

		void CalculateVertexIndices(void);

	public:

		AabbBvhLeafData(void) : containingLeafPtr(0), triangleIndicesPtr(0), numTriangleIndices(0), vertexIndicesPtr(0), leafNumber(-1) {}
		AabbBvhLeafData(unsigned int* _triangleIndicesPtr, unsigned int _numTriangleIndices, const EDCollision::Aabb& _aabb);
		AabbBvhLeafData(unsigned int* _triangleIndicesPtr, unsigned int _numTriangleIndices, unsigned int* vertIndicesPtr, const EDCollision::Aabb& _aabb);
		virtual ~AabbBvhLeafData(void);
		bool SplitData(EDTreeInternalNodeData*& splitter, EDTreeLeafNodeData*& resultPtr, unsigned int& newDataCount);
		AabbBvhLeafNode* CreateLeafNode(EDTree *ownerTreePtr);
		AabbBvhLeafNode* GetContainingLeaf(void) const;
		const EDCollision::Aabb& GetBounds(void)const { return aabb; }
		unsigned int* GetTriangleIndices(void){ return triangleIndicesPtr; }
		unsigned int GetNumTriangleIndices(void){ return numTriangleIndices; }
		void SetTriIndices(const unsigned int* indicesPtr, unsigned int numIndices);
		const unsigned int* GetVertexIndices(void) const;
		unsigned int GetVertexIndexCount(void)const{ return numTriangleIndices*3; }
		int GetLeafNumber(void)const{ return leafNumber; }
		void SetLeafNumber(int _leafNumber){ leafNumber = _leafNumber; }
	};

private:
	AabbBvhLeafData* leafDataPtr;
	EDTree* ownerTreePtr;

public:
	AabbBvhLeafNode(void) : leafDataPtr(0), ownerTreePtr(0) {}
	
	AabbBvhInternalNode* CreateInternalNode(void) const
	{
		return new AabbBvhInternalNode;
	}

	~AabbBvhLeafNode()
	{
		delete leafDataPtr;
	}

	AabbBvhLeafData* GetLeafNodeData(void) const
	{
		return leafDataPtr;
	}

	void SetLeafNodeData(EDTreeLeafNodeData* dataPtr)
	{
		leafDataPtr = (AabbBvhLeafData*)dataPtr;
	}

	EDTree* GetOwnerTree(void){ return ownerTreePtr; }
	void SetOwnerTree(EDTree* treePtr){ ownerTreePtr = treePtr; }

	void Traverse( void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, void* traversalDataPtr, void* traversalFuncResultPtr ) ) const;

	const EDCollision::Aabb* GetBounds(void)const { return (leafDataPtr == 0 ? 0 : &(leafDataPtr->GetBounds())); }
};