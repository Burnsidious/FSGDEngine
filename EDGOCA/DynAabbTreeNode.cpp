#include "precompiled.h"
#include "DynAabbTreeNode.h"
#include "GameObject.h"
#include "../EDRendererD3D/VisibilityQueryInterface.h"

using namespace EDRendererD3D;

namespace EDGOCA
{
	DynAabbTreeNode::DynAabbTreeNode(void) : parent(0)
	{
		AABB.min.x = AABB.min.y = AABB.min.z = FLT_MAX;
		AABB.max.x = AABB.max.y = AABB.max.z = -FLT_MAX;
		occQueryInterface = 0;
	}

	DynAabbTreeInternalNode::~DynAabbTreeInternalNode()
	{
		if( children[0] != 0 )
		{
			if( children[0]->GetDynAabbTreeNodeType() == INTERNAL )
				delete children[0];
		}

		if( children[1] != 0 )
		{
			if( children[1]->GetDynAabbTreeNodeType() == INTERNAL )
				delete children[1];
		}

	}

	int DynAabbTreeInternalNode::NodeSide(DynAabbTreeNode* node)
	{
		if( children[0] == node )
			return 0;
		if( children[1] == node )
			return 1;

		return -1;
	}

	DynAabbTreeLeafNode::DynAabbTreeLeafNode(GameObject* gameObject) : gameObject(gameObject) 
	{ 
		tree = 0;
		dynAabbTreeNodeType = LEAF;
		AABB = gameObject->GetAabb();
	}
}