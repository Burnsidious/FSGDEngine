#pragma once

#include "../EDCollision/CollidableTypes.h"



#include "DynAabbTreeNode.h"
#include "MessageT.h"

namespace EDGOCA
{
	class IMessage;
	
	typedef void (*PerOverlappedObjectFunc)(GameObject* gameObjectPtr, void* voidPtr);

	class DynAabbTree
	{
	//	static unsigned int frameID;

	//	DynAabbTreeNode* root;

	//	DynAabbTreeNode* Insert(DynAabbTreeNode* insertPoint, DynAabbTreeLeafNode* insertion);

	//	static float Cost(DynAabbTreeNode* first, DynAabbTreeNode* second);

	//	std::set<DynAabbTreeLeafNode*> leafList;

	//	static void TreeCollision(DynAabbTreeNode* nodeA, DynAabbTreeNode* nodeB);
	//	static void SelfTreeCollision(DynAabbTreeLeafNode* leafPtr, DynAabbTreeNode* node);

	//	IOcclusionQueryCreator occlusionQueryCreator;

	//	//static void FrustumAndOcclusionCull(DynAabbTreeNode *node, DynAabbTreeNode*& queryQueueHead, DynAabbTreeNode*& queryQueueTail, EDCollision::FrustumHull* frustum, IMessage* msg);
	//	//static void FrustumCull(DynAabbTreeNode* node, EDCollision::FrustumHull* frustum, IMessage* msg);

	//	//static void ForAllOverlappedObjects(DynAabbTreeNode* node, PerOverlappedObjectFunc perObjFunc, const EDCollision::Segment& segment, void* voidPtr );
	//	//static void ForAllOverlappedObjects(DynAabbTreeNode* node, PerOverlappedObjectFunc perObjFunc, const EDCollision::Sphere& sphere, void* voidPtr );
	//	//static void ForAllOverlappedObjects(DynAabbTreeNode* node, PerOverlappedObjectFunc perObjFunc, const EDCollision::Aabb& aabb, void* voidPtr );
	//	//static void ForAllOverlappedObjects(DynAabbTreeNode* node, PerOverlappedObjectFunc perObjFunc, const EDCollision::FrustumHull& frustum, void* voidPtr );

	//public:
	//	DynAabbTree(void) : root(0), occlusionQueryCreator(0) {}

	//	~DynAabbTree(void);
	//
	//	DynAabbTreeNode* Insert(DynAabbTreeLeafNode* insertion);

	//	DynAabbTreeNode* Remove(DynAabbTreeLeafNode* leaf);

	//	void Update(DynAabbTreeLeafNode* leaf, const EDCollision::Aabb& newBounds);

	//	static void TreeCollision(DynAabbTree* treeA, DynAabbTree* treeB);

	//	//static void FrustumCull(DynAabbTree* tree, EDCollision::FrustumHull* frustum, IMessage* msg);
	//	//static void FrustumAndOcclusionCull(DynAabbTree* tree, EDCollision::FrustumHull* frustum, IMessage* msg);

	//	//static void ForAllOverlappedObjects(DynAabbTree* tree, PerOverlappedObjectFunc perObjFunc, const EDCollision::Segment& segment, void* voidPtr );
	//	//static void ForAllOverlappedObjects(DynAabbTree* tree, PerOverlappedObjectFunc perObjFunc, const EDCollision::Sphere& sphere, void* voidPtr );
	//	//static void ForAllOverlappedObjects(DynAabbTree* tree, PerOverlappedObjectFunc perObjFunc, const EDCollision::Aabb& aabb, void* voidPtr );
	//	//static void ForAllOverlappedObjects(DynAabbTree* tree, PerOverlappedObjectFunc perObjFunc, const EDCollision::FrustumHull& frustum, void* voidPtr );

	//	void SetOcclusionQueryCreator(IOcclusionQueryCreator creator);
	//	IOcclusionQueryCreator GetOcclusionQueryCreator(void){ return occlusionQueryCreator; }
	};
}