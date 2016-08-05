#include "precompiled.h"
#include "DynAabbTreeNode.h"
#include "DynAabbTree.h"
#include "GameObject.h"
#include "../EDCollision/Collision.h"

#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDCollision/sphere.h"



using namespace std;
using namespace EDMath;
using namespace EDCollision;

namespace EDGOCA
{
	unsigned int DynAabbTree::frameID = 0;

	float DynAabbTree::Cost(DynAabbTreeNode* first, DynAabbTreeNode* second)
	{
		Float3 c0 = (first->AABB.max + first->AABB.min) * 0.5f;
		Float3 c1 = (second->AABB.max + second->AABB.min) * 0.5f;
		Float3 d = c0 - c1;

		return abs(d.x) + abs(d.y) + abs(d.z);
	}

	DynAabbTree::~DynAabbTree(void)
	{
		if( root != 0 )
			if( root->GetDynAabbTreeNodeType() == DynAabbTreeNode::INTERNAL )
				delete root;
	}

	DynAabbTreeNode* DynAabbTree::Insert(DynAabbTreeLeafNode* insertion)
	{
		return Insert( root, insertion );
	}

	DynAabbTreeNode* DynAabbTree::Insert(DynAabbTreeNode* insertPoint, DynAabbTreeLeafNode* insertion)
	{
		//insertion->tree = this;

		//leafList.insert(insertion);

		//if( occlusionQueryCreator != 0 && insertion->occQueryInterface == 0 )
		//	insertion->occQueryInterface = occlusionQueryCreator();

		//if( root == 0 )
		//{
		//	insertion->parent = 0;
		//	root = insertion;
		//	
		//	return insertion;
		//}

		//if( insertPoint == 0 )
		//	insertPoint = root;

		//float insertionVolume = (insertion->GetAABB().max.x - insertion->GetAABB().min.x)*(insertion->GetAABB().max.y - insertion->GetAABB().min.y)*(insertion->GetAABB().max.x - insertion->GetAABB().min.x);

		//while( insertPoint->GetDynAabbTreeNodeType() == DynAabbTreeNode::INTERNAL )
		//{
		//	DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)insertPoint;

		//	float insertionPointVolume = (pInternal->GetAABB().max.x - pInternal->GetAABB().min.x)*(pInternal->GetAABB().max.y - pInternal->GetAABB().min.y)*(pInternal->GetAABB().max.x - pInternal->GetAABB().min.x);

		//	//if( insertionVolume / insertionPointVolume > 0.125f )
		//	//	break;

		//	float cost0 = Cost( insertion, pInternal->children[0] );
		//	float cost1 = Cost( insertion, pInternal->children[1] );

		//	if( cost0 <= cost1  )
		//	{
		//		insertPoint = pInternal->children[0];
		//	}
		//	else
		//	{
		//		insertPoint = pInternal->children[1];
		//	}

		//	for(unsigned int axis = 0; axis < 3; ++axis)
		//	{
		//		//if( pInternal->GetAABB().min.v[axis] > insertion->GetAABB().min.v[axis] )
		//		//	pInternal->AABB.min.v[axis] = insertion->AABB.min.v[axis];
		//		//if( pInternal->GetAABB().max.v[axis] < insertion->GetAABB().max.v[axis] )
		//		//	pInternal->AABB.max.v[axis] = insertion->AABB.max.v[axis];
		//	}
		//}

		//DynAabbTreeInternalNode* pNewInternal = new DynAabbTreeInternalNode();

		//pNewInternal->parent = insertPoint->parent;
		//pNewInternal->children[0] = insertion;
		//pNewInternal->children[1] = insertPoint;

		//pNewInternal->children[0]->parent = pNewInternal;
		//pNewInternal->children[1]->parent = pNewInternal;

		//if( occlusionQueryCreator != 0 && pNewInternal->occQueryInterface == 0 )
		//	pNewInternal->occQueryInterface = occlusionQueryCreator();

		//for(unsigned int childIndex = 0; childIndex < 2; ++childIndex)
		//{
		//	for(unsigned int axis = 0; axis < 3; ++axis)
		//	{
		//		if( pNewInternal->GetAABB().min.v[axis] > pNewInternal->children[childIndex]->GetAABB().min.v[axis] )
		//			pNewInternal->AABB.min.v[axis] = pNewInternal->children[childIndex]->AABB.min.v[axis];
		//		if( pNewInternal->GetAABB().max.v[axis] < pNewInternal->children[childIndex]->GetAABB().max.v[axis] )
		//			pNewInternal->AABB.max.v[axis] = pNewInternal->children[childIndex]->AABB.max.v[axis];
		//	}
		//}

		//if( pNewInternal->parent != 0 )
		//	pNewInternal->parent->children[ pNewInternal->parent->NodeSide(insertPoint) ] = pNewInternal;
		//else
		//	root = pNewInternal;

		return insertion;
	}

	DynAabbTreeNode* DynAabbTree::Remove(DynAabbTreeLeafNode* leaf)
	{
		//leafList.erase(leaf);

		//leaf->tree = 0;

		//if( leaf == root )
		//{
		//	root = 0;
		//	return 0;
		//}

		//DynAabbTreeNode* sibling;

		//if( leaf->parent->NodeSide(leaf) == 0 )
		//	sibling = leaf->parent->children[1];
		//else
		//	sibling = leaf->parent->children[0];

		//leaf->parent->children[0] = 0;
		//leaf->parent->children[1] = 0;

		//DynAabbTreeInternalNode* pGrandParent = leaf->parent->parent;

		//if( pGrandParent == 0 )
		//	root = sibling;
		//else
		//	pGrandParent->children[ pGrandParent->NodeSide( leaf->parent ) ] = sibling;

		//sibling->parent = pGrandParent;

		//delete leaf->parent;
		//leaf->parent = 0;

		//while( pGrandParent != 0 )
		//{
		//	EDCollision::Aabb vol = pGrandParent->AABB;
		//
		//	pGrandParent->AABB.min = Float3( FLT_MAX, FLT_MAX, FLT_MAX );
		//	pGrandParent->AABB.max = Float3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		//
		//	for(unsigned int childIndex = 0; childIndex < 2; ++childIndex)
		//	{
		//		for(unsigned int axis = 0; axis < 3; ++axis)
		//		{
		//			if( pGrandParent->GetAABB().min.v[axis] > pGrandParent->children[childIndex]->GetAABB().min.v[axis] )
		//				pGrandParent->AABB.min.v[axis] = pGrandParent->children[childIndex]->AABB.min.v[axis];
		//			if( pGrandParent->GetAABB().max.v[axis] < pGrandParent->children[childIndex]->GetAABB().max.v[axis] )
		//				pGrandParent->AABB.max.v[axis] = pGrandParent->children[childIndex]->AABB.max.v[axis];				
		//		}
		//	}

		//	if( vol.min.x == pGrandParent->GetAABB().min.x && vol.min.y == pGrandParent->GetAABB().min.y && vol.min.z == pGrandParent->GetAABB().min.z &&
		//		vol.max.x == pGrandParent->GetAABB().max.x && vol.max.y == pGrandParent->GetAABB().max.y && vol.max.z == pGrandParent->GetAABB().max.z )
		//		return pGrandParent;
		//	else
		//		pGrandParent = pGrandParent->parent;
		//}

		//return pGrandParent;

		return 0;
	}

	void DynAabbTree::Update(DynAabbTreeLeafNode* leaf, const EDCollision::Aabb& newBounds)
	{
		//Float3 cfirst = (leaf->GetAABB().max + leaf->GetAABB().min) * 0.5f;
		//Float3 csecond = (newBounds.max + newBounds.min) * 0.5f;
	
		//Float3 velocity = csecond - cfirst;
		//const float predictedFrameCount = 0.0f;
		//const float margin = 0.0f;

		//EDCollision::Aabb expandedBounds = newBounds;
		//expandedBounds.min -= Float3(margin, margin, margin);
		//expandedBounds.max += Float3(margin, margin, margin);

		//velocity *= predictedFrameCount;

		//expandedBounds.max.x += abs(velocity.x);
		//expandedBounds.min.x -= abs(velocity.x);

		//expandedBounds.max.y += abs(velocity.y);
		//expandedBounds.min.y -= abs(velocity.y);

		//expandedBounds.max.z += abs(velocity.z);
		//expandedBounds.min.z -= abs(velocity.z);

		//unsigned int axis = 0;
		//for(; axis < 3; ++axis)
		//{
		//	if( expandedBounds.max.v[axis] > leaf->AABB.max.v[axis] )
		//		break;
		//	if( expandedBounds.min.v[axis] < leaf->AABB.min.v[axis] )
		//		break;
		//}

		//if( axis == 3 )
		//	return;

		//DynAabbTreeNode* localRoot = 0;

		//localRoot = Remove( leaf );
		//
		//leaf->AABB = expandedBounds;
		//leaf->tree = 0;
		//Insert( localRoot, leaf );
	}

	void DynAabbTree::TreeCollision(DynAabbTreeNode* nodeA, DynAabbTreeNode* nodeB)
	{
		//if( !AABBAABBCollide( nodeA->GetAABB(), nodeB->GetAABB() ) )
		//	return;

		//DynAabbTreeNode::DynAabbTreeNodeType typeA = nodeA->GetDynAabbTreeNodeType();
		//DynAabbTreeNode::DynAabbTreeNodeType typeB = nodeB->GetDynAabbTreeNodeType();

		//if( typeA == DynAabbTreeNode::LEAF && typeB == DynAabbTreeNode::LEAF )
		//{
		//	DynAabbTreeLeafNode* pLeafA = (DynAabbTreeLeafNode*)nodeA;
		//	DynAabbTreeLeafNode* pLeafB = (DynAabbTreeLeafNode*)nodeB;

		//	GameObject::CollideObjects( pLeafA->gameObject, pLeafB->gameObject );
		//}
		//else
		//{
		//	Float3 deltaA = nodeA->GetAABB().max - nodeA->GetAABB().min;
		//	Float3 deltaB = nodeB->GetAABB().max - nodeB->GetAABB().min;
		//	float volA = deltaA.x*deltaA.y*deltaA.z;
		//	float volB = deltaB.x*deltaB.y*deltaB.z;

		//	if( typeB == DynAabbTreeNode::LEAF || (typeA == DynAabbTreeNode::INTERNAL && (volA >= volB)))
		//	{
		//		DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)nodeA;
		//		TreeCollision( pInternal->children[0], nodeB );
		//		TreeCollision( pInternal->children[1], nodeB );				
		//	}
		//	else
		//	{
		//		DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)nodeB;
		//		TreeCollision( nodeA, pInternal->children[0] );
		//		TreeCollision( nodeA, pInternal->children[1] );				
		//	}
		//}
	}

	void DynAabbTree::SelfTreeCollision(DynAabbTreeLeafNode* leafPtr, DynAabbTreeNode* node)
	{
		//if( !AABBAABBCollide( leafPtr->GetAABB(), node->GetAABB() ) )
		//	return;

		//DynAabbTreeNode::DynAabbTreeNodeType typeB = node->GetDynAabbTreeNodeType();

		//if( typeB == DynAabbTreeNode::LEAF )
		//{
		//	DynAabbTreeLeafNode* pLeafB = (DynAabbTreeLeafNode*)node;
		//	if( pLeafB > leafPtr )
		//		GameObject::CollideObjects( leafPtr->gameObject, pLeafB->gameObject );
		//}
		//else
		//{
		//	DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)node;
		//	SelfTreeCollision(leafPtr, pInternal->children[0] );
		//	SelfTreeCollision(leafPtr, pInternal->children[1] );
		//}
	}

	void DynAabbTree::TreeCollision(DynAabbTree* treeA, DynAabbTree* treeB)
	{
		//if( treeA == treeB )
		//{
		//	set<DynAabbTreeLeafNode*>::iterator leafIter = treeA->leafList.begin();

		//	for(; leafIter != treeA->leafList.end(); ++leafIter )
		//		SelfTreeCollision( (*leafIter), treeA->root );
		//}
		//else if( treeA->root != 0 && treeB->root != 0 )
		//	TreeCollision(treeA->root, treeB->root);
	}

	void DynAabbTree::SetOcclusionQueryCreator(IOcclusionQueryCreator creator)
	{ 
/*		if( occlusionQueryCreator != 0 && root != 0 )
		{
			list<DynAabbTreeNode*> nodeList;
			nodeList.push_back( root );

			while( !nodeList.empty() )
			{
				DynAabbTreeNode* currentNode = nodeList.front();
				nodeList.pop_front();

				delete currentNode->occQueryInterface;
				currentNode->occQueryInterface = creator();

				if( currentNode->GetDynAabbTreeNodeType() == DynAabbTreeNode::INTERNAL )
				{
					DynAabbTreeInternalNode* internalNode = (DynAabbTreeInternalNode*)currentNode;
					nodeList.push_back(internalNode->children[0]);
					nodeList.push_back(internalNode->children[1]);
				}
			}
		}

		occlusionQueryCreator = creator;	*/	
	}

	void DynAabbTree::FrustumAndOcclusionCull(DynAabbTreeNode *node, DynAabbTreeNode*& queryQueueHead, DynAabbTreeNode*& queryQueueTail, FrustumHull* frustum, IMessage* msg)
	{
#ifdef CHECK_QUEUE
		// Check's the queryQueue for completed queries during traversal.
		// Part of the original algorithm as described in GPU Gems 2.
		// Good idea in theory, but seems to actually hurt performance.
		// If GetQueryResult doesn't cause a flush then this will improve
		// performance in that case, but overall performance still seems
		// slightly worse than flushing and not doing this.
		while( queryQueueHead != 0 )
		{
			int queryResult = queryQueueHead->occQueryInterface->GetQueryResult();

			if( queryResult == 0 )
				break;

			DynAabbTreeNode* queryNode = queryQueueHead;
			queryQueueHead = queryQueueHead->occQueryInterface->GetNextQueryNode();
			queryNode->occQueryInterface->SetNextQueryNode(0);

			if( queryNode == queryQueueTail )
				queryQueueTail = 0;

			if( queryResult == 2 )
				break;

			DynAabbTreeNode* visibleNode = queryNode;
			while( !visibleNode->occQueryInterface->GetVisibleState() )
			{
				visibleNode->occQueryInterface->SetVisibleState(true);
				visibleNode = visibleNode->parent;

				if( visibleNode == 0 )
					break;
			}

			if( queryNode->GetDynAabbTreeNodeType() == DynAabbTreeNode::LEAF )
			{
				DynAabbTreeLeafNode* leafPtr = (DynAabbTreeLeafNode*)queryNode;

				leafPtr->gameObject->OnMessage( msg );
			}
			else
			{
				DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)queryNode;
				FrustumCull( pInternal->children[0], queryQueueHead, queryQueueTail, msg, frameID );
				FrustumCull( pInternal->children[1], queryQueueHead, queryQueueTail, msg, frameID );
			}
		}
#endif

		//EDCollision::Aabb tempAabb = node->GetAABB();

		//if( frustum->Collides( &tempAabb ) )
		//{
		//	bool clipsNearPlane = false;
		//	Float3 boxCenter = (tempAabb.max + tempAabb.min) * 0.5f;
		//	Float3 extents = tempAabb.max - boxCenter;

		//	Float3 absN = frustum->planes[0].normal;
		//	absN.x = abs( absN.x );
		//	absN.y = abs( absN.y );
		//	absN.z = abs( absN.z );

		//	float projR = DotProduct( absN, extents );

		//	if( DotProduct( boxCenter, frustum->planes[0].normal ) - frustum->planes[0].offset < projR )
		//		clipsNearPlane = true;

		//	bool wasVisible = node->occQueryInterface->GetVisibleState() &&
		//						(node->occQueryInterface->GetLastVisitedFlag() == frameID - 1 );

		//	bool leafOrWasInvisible = !wasVisible || (node->GetDynAabbTreeNodeType() == DynAabbTreeNode::LEAF);

		//	node->occQueryInterface->SetVisibleState(false);
		//	node->occQueryInterface->SetLastVisitedFlag(frameID);

		//	// If the bounds clip the near plane then the object
		//	// should not be occluded, but would fail the occlusion test.
		//	// So flag it and all parents as visible, and don't issue a query.
		//	if( clipsNearPlane )
		//	{
		//		DynAabbTreeNode* visibleNode = node;
		//		while( !visibleNode->occQueryInterface->GetVisibleState() )
		//		{
		//			visibleNode->occQueryInterface->SetVisibleState(true);
		//			visibleNode = visibleNode->parent;

		//			if( visibleNode == 0 )
		//				break;
		//		}
		//	}
		//	
		//	if( leafOrWasInvisible && !clipsNearPlane )
		//	{
		//		node->occQueryInterface->Issue(tempAabb);

		//		if( queryQueueTail != 0 )
		//		{
		//			queryQueueTail->occQueryInterface->SetNextQueryNode(node);
		//			queryQueueTail = node;
		//		}
		//		else
		//		{
		//			queryQueueHead = node;
		//			queryQueueTail = node;
		//		}
		//	}

		//	if( wasVisible || clipsNearPlane )
		//	{
		//		if( node->GetDynAabbTreeNodeType() == DynAabbTreeNode::LEAF )
		//		{
		//			DynAabbTreeLeafNode* leafPtr = (DynAabbTreeLeafNode*)node;

		//			EDMath::Sphere tempSphere;
		//			tempSphere.center = (tempAabb.min + tempAabb.max) * 0.5f;
		//			tempSphere.radius = 0.5f;

		//			leafPtr->gameObject->OnMessage( msg );
		//		}
		//		else
		//		{
		//			DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)node;

		//			FrustumAndOcclusionCull( pInternal->children[0], queryQueueHead, queryQueueTail, frustum, msg);
		//			FrustumAndOcclusionCull( pInternal->children[1], queryQueueHead, queryQueueTail, frustum, msg);
		//		}
		//	}
		//}
	}

	void DynAabbTree::FrustumAndOcclusionCull(DynAabbTree* tree, FrustumHull* frustum, IMessage* msg)
	{
		//if( tree->root == 0 )
		//	return;

		//++frameID;
		//DynAabbTreeNode* queueHead = 0;
		//DynAabbTreeNode* queueTail = 0;

		//FrustumAndOcclusionCull(tree->root, queueHead, queueTail, frustum, msg );

		//while( queueHead != 0 )
		//{
		//	DynAabbTreeNode* queryNode = queueHead;
		//	queueHead = queryNode->occQueryInterface->GetNextQueryNode();
		//	queryNode->occQueryInterface->SetNextQueryNode(0);

		//	if( queryNode == queueTail )
		//		queueTail = 0;

		//	int result = 0;

		//	while( result == 0 )
		//		result = queryNode->occQueryInterface->GetQueryResult();

		//	if( result != 1 )
		//		continue;

		//	DynAabbTreeNode* visibleNode = queryNode;
		//	while( !visibleNode->occQueryInterface->GetVisibleState() )
		//	{
		//		visibleNode->occQueryInterface->SetVisibleState(true);
		//		visibleNode = visibleNode->parent;

		//		if( visibleNode == 0 )
		//			break;
		//	}

		//	if( queryNode->GetDynAabbTreeNodeType() == DynAabbTreeNode::LEAF )
		//	{
		//		DynAabbTreeLeafNode* leafPtr = (DynAabbTreeLeafNode*)queryNode;

		//		leafPtr->gameObject->OnMessage( msg );
		//	}
		//	else
		//	{
		//		DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)queryNode;
		//		FrustumAndOcclusionCull( pInternal->children[0], queueHead, queueTail, frustum, msg );
		//		FrustumAndOcclusionCull( pInternal->children[1], queueHead, queueTail, frustum, msg );
		//	}
		//}
	}

	void DynAabbTree::FrustumCull(DynAabbTree* tree, FrustumHull* frustum, IMessage* msg)
	{
		//FrustumCull( tree->root, frustum, msg );
	}

	void DynAabbTree::ForAllOverlappedObjects(DynAabbTree* tree, PerOverlappedObjectFunc perObjFunc, const EDCollision::Segment& segment, void* voidPtr )
	{
		//ForAllOverlappedObjects( tree->root, perObjFunc, segment, voidPtr ); 
	}

	void DynAabbTree::ForAllOverlappedObjects(DynAabbTree* tree, PerOverlappedObjectFunc perObjFunc, const EDCollision::Sphere& sphere, void* voidPtr )
	{
		//ForAllOverlappedObjects( tree->root, perObjFunc, sphere, voidPtr );
	}

	void DynAabbTree::ForAllOverlappedObjects(DynAabbTree* tree, PerOverlappedObjectFunc perObjFunc, const EDCollision::Aabb& aabb, void* voidPtr )
	{
		//ForAllOverlappedObjects( tree->root, perObjFunc, aabb, voidPtr );
	}

	void DynAabbTree::ForAllOverlappedObjects(DynAabbTree* tree, PerOverlappedObjectFunc perObjFunc, const FrustumHull& frustum, void* voidPtr )
	{
		//ForAllOverlappedObjects( tree->root, perObjFunc, frustum, voidPtr );
	}

	//void DynAabbTree::FrustumCull(DynAabbTreeNode* node, FrustumHull* frustum, IMessage* msg)
	//{
	//	if(!node)
	//		return;
	//	assert(node != 0);

	//	EDCollision::Aabb tempAabb = node->GetAABB();

	//	if( !frustum->Collides( &tempAabb ) )
	//		return;

	//	if( node->GetDynAabbTreeNodeType() == DynAabbTreeNode::INTERNAL )
	//	{
	//		DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)node;

	//		FrustumCull( pInternal->children[0], frustum, msg );
	//		FrustumCull( pInternal->children[1], frustum, msg );
	//	}
	//	else
	//	{
	//		DynAabbTreeLeafNode* leafPtr = (DynAabbTreeLeafNode*)node;

	//		leafPtr->gameObject->OnMessage( msg );
	//	}
	//}

	//void DynAabbTree::ForAllOverlappedObjects(DynAabbTreeNode* node, PerOverlappedObjectFunc perObjFunc, const EDCollision::Segment& segment, void* voidPtr )
	//{
	//	assert(node != 0);

	//	EDCollision::Aabb tempAabb = node->GetAABB();
	//	
	//	if( !EDCollision::AABBSegmentCollide( segment.start, segment.end, tempAabb.min, tempAabb.max ) )
	//		return;

	//	if( node->GetDynAabbTreeNodeType() == DynAabbTreeNode::INTERNAL )
	//	{
	//		DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)node;
	//		
	//		ForAllOverlappedObjects( pInternal->children[0], perObjFunc, segment, voidPtr );
	//		ForAllOverlappedObjects( pInternal->children[1], perObjFunc, segment, voidPtr );
	//	}
	//	else
	//	{
	//		DynAabbTreeLeafNode* leafPtr = (DynAabbTreeLeafNode*)node;
	//		
	//		if( (leafPtr->gameObject->GetGameObjectFlags() & GameObject::DELETING) != GameObject::DELETING )
	//			perObjFunc( leafPtr->gameObject, voidPtr );
	//	}
	//}

	//void DynAabbTree::ForAllOverlappedObjects(DynAabbTreeNode* node, PerOverlappedObjectFunc perObjFunc, const EDCollision::Sphere& sphere, void* voidPtr )
	//{
	//	assert(node != 0);

	//	//Aabb tempAabb = node->GetAABB();
	//	//
	//	////if( !EDCollision::SphereAABBCollide( sphere, tempAabb ) )
	//	////	return;

	//	//if( node->GetDynAabbTreeNodeType() == DynAabbTreeNode::INTERNAL )
	//	//{
	//	//	DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)node;
	//	//	
	//	//	ForAllOverlappedObjects( pInternal->children[0], perObjFunc, sphere, voidPtr );
	//	//	ForAllOverlappedObjects( pInternal->children[1], perObjFunc, sphere, voidPtr );
	//	//}
	//	//else
	//	//{
	//	//	DynAabbTreeLeafNode* leafPtr = (DynAabbTreeLeafNode*)node;
	//	//	
	//	//	if( (leafPtr->gameObject->GetGameObjectFlags() & GameObject::DELETING) != GameObject::DELETING )
	//	//		perObjFunc( leafPtr->gameObject, voidPtr );
	//	//}
	//}

	//void DynAabbTree::ForAllOverlappedObjects(DynAabbTreeNode* node, PerOverlappedObjectFunc perObjFunc, const EDCollision::Aabb& aabb, void* voidPtr )
	//{
	//	assert(node != 0);

	//	//Aabb tempAabb = node->GetAABB();
	//	//
	//	//if( !EDCollision::AABBAABBCollide( aabb, tempAabb ) )
	//	//	return;

	//	//if( node->GetDynAabbTreeNodeType() == DynAabbTreeNode::INTERNAL )
	//	//{
	//	//	DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)node;
	//	//	
	//	//	ForAllOverlappedObjects( pInternal->children[0], perObjFunc, aabb, voidPtr );
	//	//	ForAllOverlappedObjects( pInternal->children[1], perObjFunc, aabb, voidPtr );
	//	//}
	//	//else
	//	//{
	//	//	DynAabbTreeLeafNode* leafPtr = (DynAabbTreeLeafNode*)node;

	//	//	if( (leafPtr->gameObject->GetGameObjectFlags() & GameObject::DELETING) != GameObject::DELETING )
	//	//		perObjFunc( leafPtr->gameObject, voidPtr );
	//	//}
	//}

	//void DynAabbTree::ForAllOverlappedObjects(DynAabbTreeNode* node, PerOverlappedObjectFunc perObjFunc, const FrustumHull& frustum, void* voidPtr )
	//{
	//	assert(node != 0);

	//	//Aabb tempAabb = node->GetAABB();
	//	//
	//	//for(int i = 0; i < 6; ++i)
	//	//	if( tempAabb.ClassifyToPlane( frustum.planes[i] ) == 0 )
	//	//		return;

	//	//if( node->GetDynAabbTreeNodeType() == DynAabbTreeNode::INTERNAL )
	//	//{
	//	//	DynAabbTreeInternalNode* pInternal = (DynAabbTreeInternalNode*)node;
	//	//	
	//	//	ForAllOverlappedObjects( pInternal->children[0], perObjFunc, frustum, voidPtr );
	//	//	ForAllOverlappedObjects( pInternal->children[1], perObjFunc, frustum, voidPtr );
	//	//}
	//	//else
	//	//{
	//	//	DynAabbTreeLeafNode* leafPtr = (DynAabbTreeLeafNode*)node;

	//	//	if( (leafPtr->gameObject->GetGameObjectFlags() & GameObject::DELETING) != GameObject::DELETING )
	//	//		perObjFunc( leafPtr->gameObject, voidPtr );
	//	//}
	//}
}
