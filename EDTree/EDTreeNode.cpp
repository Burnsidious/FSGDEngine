#include "EDTree.h"
#include "EDTreeNode.h"

using std::list;
void EDTreeNode::SetParent(EDTreeNode *_parentPtr)
{
	if( parentPtr != 0 )
	{
		if( prevSiblingPtr != 0 )
			prevSiblingPtr->nextSiblingPtr = nextSiblingPtr;
		else
			parentPtr->firstChildPtr = nextSiblingPtr;

		if( nextSiblingPtr != 0 )
			nextSiblingPtr->prevSiblingPtr = prevSiblingPtr;
		else
			parentPtr->lastChildPtr = prevSiblingPtr;
	}

	parentPtr = _parentPtr;

	if( parentPtr == 0 )
	{
		prevSiblingPtr = 0;
		nextSiblingPtr = 0;
		return;
	}

	if( parentPtr->lastChildPtr != 0 )
	{
		parentPtr->lastChildPtr->nextSiblingPtr = this;
		prevSiblingPtr = parentPtr->lastChildPtr;
		parentPtr->lastChildPtr = this;
		nextSiblingPtr = 0;
	}
	else
	{
		prevSiblingPtr = 0;
		nextSiblingPtr = 0;
		parentPtr->firstChildPtr = this;
		parentPtr->lastChildPtr = this;
	}

	//if( parentPtr != 0 )
	//{
	//	EDTreeNode* pPrevSibling = 0;
	//	EDTreeNode* pCurrentChild = parentPtr->firstChildPtr;

	//	while(pCurrentChild != 0)
	//	{
	//		if( pCurrentChild == this )
	//		{
	//			if( pPrevSibling != 0 )
	//				pPrevSibling->nextSiblingPtr = pCurrentChild->nextSiblingPtr;
	//			else
	//				parentPtr->firstChildPtr = pCurrentChild->nextSiblingPtr;

	//			break;
	//		}

	//		pPrevSibling = pCurrentChild;
	//		pCurrentChild = pCurrentChild->nextSiblingPtr;
	//	}
	//}

	//parentPtr = _parentPtr;
	//nextSiblingPtr = parentPtr->firstChildPtr;
	//parentPtr->firstChildPtr = this;

	////parentPtr = _parentPtr;
	////nextSiblingPtr = 0;

	////if( parentPtr == 0 )
	////	return;

	////EDTreeNode* pPrevChild = 0;
	////EDTreeNode* pCurrentChild = parentPtr->firstChildPtr;

	////while(pCurrentChild != 0)
	////{
	////	pPrevChild = pCurrentChild;
	////	pCurrentChild = pCurrentChild->nextSiblingPtr;
	////}

	////if( pPrevChild != 0 )
	////	pPrevChild->nextSiblingPtr = this;
	////else
	////	parentPtr->firstChildPtr = this;
}

EDTreeNode* EDTreeLeafNode::Split(void)
{
	EDTreeLeafNodeData* leafDataPtr = GetLeafNodeData();
	EDTreeLeafNodeData* newDataPtr = 0;
	EDTreeInternalNodeData* splitter = 0;

	unsigned int newDataCount = 0;
			
	if( !leafDataPtr->SplitData( splitter, newDataPtr, newDataCount ) )
	{
		this->GetOwnerTree()->StoreLeaf(this);
		nodeNumber = this->GetOwnerTree()->GetNumberNodes();
		this->GetOwnerTree()->SetNumberNodes( nodeNumber + 1 );

		return this;
	}

	//EDTreeInternalNode* newParentPtr = CreateInternalNode();
	//newParentPtr->SetInternalNodeData( splitter );
	//newParentPtr->SetParent( this->parentPtr );

	//SetParent(newParentPtr);
	//Split();

	//for(unsigned int index = 0; index < newDataCount; ++index )
	//{
	//	EDTreeLeafNode* newLeafPtr = newDataPtr[index].CreateLeafNode(this->GetOwnerTree());
	//	newLeafPtr->SetParent( newParentPtr );
	//	newLeafPtr->Split();
	//}

	EDTreeInternalNode* newParentPtr = CreateInternalNode();
	newParentPtr->SetInternalNodeData( splitter );
	
	EDTreeNode* subRootPtr = Split();
	subRootPtr->SetParent( newParentPtr );

	for(unsigned int index = 0; index < newDataCount; ++index )
	{
		EDTreeLeafNode* newLeafPtr = newDataPtr[index].CreateLeafNode(this->GetOwnerTree());
		subRootPtr = newLeafPtr->Split();
		subRootPtr->SetParent( newParentPtr );
	}

	newParentPtr->SetNodeNumber( this->GetOwnerTree()->GetNumberNodes() );
	this->GetOwnerTree()->SetNumberNodes( newParentPtr->GetNodeNumber() + 1 );

	//for(int index = newDataCount - 1; index > -1; --index)
	//{
	//	EDTreeLeafNode* newLeafPtr = newDataPtr[index].CreateLeafNode(this->GetOwnerTree());
	//	EDTreeNode* subRootPtr = newLeafPtr->Split();
	//	subRootPtr->SetParent( newParentPtr );
	//}

	//EDTreeNode* subRootPtr = Split();
	//subRootPtr->SetParent( newParentPtr );
	

	return newParentPtr; 
}

void EDTreeNode::TraverseDFPost( void* traversalDataPtr, void (*TraversalFunc)(EDTreeNode*, void* traversalDataPtr) )
{
	if( firstChildPtr != 0 )
	{
		firstChildPtr->TraverseDFPost( traversalDataPtr, TraversalFunc );
		
		if( firstChildPtr->nextSiblingPtr != 0 )
			firstChildPtr->nextSiblingPtr->TraverseDFPost( traversalDataPtr, TraversalFunc );
	}

	TraversalFunc(this, traversalDataPtr);
}

void EDTreeNode::EnumerateNodes(unsigned int& count)
{
	if( firstChildPtr != 0 )
	{
		firstChildPtr->EnumerateNodes(count);
		
		if( firstChildPtr->nextSiblingPtr != 0 )
			firstChildPtr->nextSiblingPtr->EnumerateNodes(count);
	}

	nodeNumber = count++;	
}

