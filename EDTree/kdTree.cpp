#include "kdTree.h"

using namespace EDMath;

void kdTree::BuildTree(EDTreeLeafNodeData* boundsPtr, float _minimumExtent)
{ 
	minimumExtent = _minimumExtent;
	EDTree::BuildTree(boundsPtr);
}

kdTree::kdTree(EDTreeLeafNodeData* boundsPtr, float _minimumExtent)
{
	BuildTree(boundsPtr, _minimumExtent);
}

kdTreeLeafNode* kdTreeLeafNode::kdTreeLeafBounds::CreateLeafNode(EDTree* ownerTreePtr)
{
	if( containingLeafPtr == 0 )
	{
		containingLeafPtr = new kdTreeLeafNode;

		containingLeafPtr->SetLeafNodeData(this);
		containingLeafPtr->SetOwnerTree(ownerTreePtr);
	}

	return containingLeafPtr;
}

kdTreeLeafNode* kdTreeLeafNode::kdTreeLeafBounds::GetContainingLeaf(void) const
{
	return containingLeafPtr;
}

bool kdTreeLeafNode::kdTreeLeafBounds::SplitData(EDTreeInternalNodeData*& splitter, EDTreeLeafNodeData*& resultPtr, unsigned int& newDataCount)
{
	unsigned int bestAxis = 0;
	float maxLen = 0.0f;
	for(unsigned int i = 0; i < 3; ++i)
	{
		float len = aabb.max.v[i] - aabb.min.v[i];
		if( len > maxLen )
		{
			maxLen = len;
			bestAxis = i;
		}
	}

	kdTree* ownerTreePtr = (kdTree*)(GetContainingLeaf()->GetOwnerTree());
	if( maxLen < ownerTreePtr->GetMinimumExtent()*2.0f )
		return false;

	kdTreeSplittingPlane* newPlanePtr = new kdTreeSplittingPlane;
	
	newPlanePtr->SetAxis(bestAxis);
	newPlanePtr->SetOffset( aabb.min.v[bestAxis] + (maxLen * 0.5f) );
	splitter = newPlanePtr;

	kdTreeLeafBounds* newBounds = new kdTreeLeafBounds;

	newBounds->aabb = aabb;
	newBounds->aabb.min.v[bestAxis] += (maxLen * 0.5f);
	aabb.max.v[bestAxis] = newBounds->aabb.min.v[bestAxis];
	resultPtr = newBounds;
	newDataCount = 1;

	return true;
}

kdTreeLeafNode::kdTreeLeafBounds::kdTreeLeafBounds(const EDCollision::Aabb &_aabb)
{ 
	aabb = _aabb;
	containingLeafPtr = 0;
	leafNumber = -1;
}

kdTreeLeafNode::kdTreeLeafBounds::kdTreeLeafBounds(const Float3& vmin, const Float3& vmax)
{ 
	containingLeafPtr = 0;
	aabb.min = vmin; 
	aabb.max = vmax;
	leafNumber = -1;
}

void kdTreeLeafNode::kdTreeLeafBounds::AddContainedItem(void* itemPtr)
{ 
	containedItemsPtr.push_back( itemPtr );
}

void kdTreeLeafNode::kdTreeLeafBounds::RemoveContainedItem(void* itemPtr)
{
	std::list< void* >::iterator iter = containedItemsPtr.begin();

	for(; iter != containedItemsPtr.end(); ++iter )
	{
		if( (*iter) == itemPtr )
		{
			containedItemsPtr.erase( iter );
			break;
		}
	}
}

void kdTreeInternalNode::Traverse( void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, void* traversalDataPtr, void* traversalFuncResultPtr ) ) const
{
	unsigned int testResult = -1;

	TraversalFunc( this, traversalDataPtr, &testResult );

	if( testResult == 0 )
		this->GetChildren()->Traverse( traversalDataPtr, TraversalFunc );
	else if( testResult == 1 )
		this->GetChildren()->GetNextSibling()->Traverse( traversalDataPtr, TraversalFunc );
	else if( testResult == 2 )
	{
		this->GetChildren()->Traverse( traversalDataPtr, TraversalFunc );
		this->GetChildren()->GetNextSibling()->Traverse( traversalDataPtr, TraversalFunc );
	}
	else if(testResult == 3 )
	{
		this->GetChildren()->GetNextSibling()->Traverse( traversalDataPtr, TraversalFunc );
		this->GetChildren()->Traverse( traversalDataPtr, TraversalFunc );
	}
}

void kdTreeLeafNode::Traverse( void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, void* traversalDataPtr, void* traversalFuncResultPtr ) ) const
{
	unsigned int testResult = 0;
	TraversalFunc( this, traversalDataPtr, &testResult );
}
