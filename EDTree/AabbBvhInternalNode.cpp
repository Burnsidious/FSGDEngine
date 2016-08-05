#include "AabbBvhInternalNode.h"

void AabbBvhInternalNode::Traverse( void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, void* traversalDataPtr, void* traversalFuncResultPtr ) ) const
{
	unsigned int testResult = 0;

	TraversalFunc( this, traversalDataPtr, &testResult );
	
	if( testResult == 0 )
		return;

	this->GetChildren()->Traverse( traversalDataPtr, TraversalFunc );
	this->GetChildren()->GetNextSibling()->Traverse( traversalDataPtr, TraversalFunc );
}

AabbBvhInternalNode::~AabbBvhInternalNode(void)
{

	delete AabbPtr;
}
