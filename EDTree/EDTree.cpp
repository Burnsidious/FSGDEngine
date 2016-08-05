#include "EDTree.h"

void EDTree::Traverse(void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, void* traversalDataPtr, void* traversalFuncResultPtr ))
{
	if( rootNodePtr != 0 )
		rootNodePtr->Traverse( traversalDataPtr, TraversalFunc );
}

void EDTree::TraverseDFPost(void* traversalDataPtr, void (*TraversalFunc)(EDTreeNode*, void* traversalDataPtr))
{
	if( rootNodePtr != 0 )
		rootNodePtr->TraverseDFPost( traversalDataPtr, TraversalFunc );
}
