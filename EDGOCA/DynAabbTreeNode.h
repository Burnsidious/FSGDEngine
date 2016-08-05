#pragma once

//#include "../EDCollision/CollidableTypes.h"
#include "../EDMath/aabb.h"

namespace EDGOCA
{
	class GameObject;

	class DynAabbTree;
	class DynAabbTreeNode;
	class DynAabbTreeInternalNode;

	class IOcclusionQueryInterface
	{
		bool visible;
		unsigned int lastVisited;
		bool clipsNearPlane;

		DynAabbTreeNode* nextQueryNode;
	public:
		IOcclusionQueryInterface(void) : visible(true), lastVisited(0), nextQueryNode(0), clipsNearPlane(false) {}
		virtual ~IOcclusionQueryInterface(){}

		virtual void Initialize(void) = 0;
		virtual void Issue(const EDMath::Aabb& aabb) = 0;
		virtual int GetQueryResult(void) = 0;

		bool GetVisibleState(void){ return visible; }
		void SetVisibleState(bool state){ visible = state; }

		unsigned int GetLastVisitedFlag(void){ return lastVisited; }
		void SetLastVisitedFlag(unsigned int lv){ lastVisited = lv; }

		DynAabbTreeNode* GetNextQueryNode(void){ return nextQueryNode; }
		void SetNextQueryNode(DynAabbTreeNode* next){ nextQueryNode = next; }

		bool GetClipState(void){ return clipsNearPlane; }
		void SetClipState(bool state){ clipsNearPlane = state; }
	};

	typedef IOcclusionQueryInterface* (*IOcclusionQueryCreator)(void);


	class DynAabbTreeNode
	{
		friend class DynAabbTree;
	public:
		enum DynAabbTreeNodeType{ INTERNAL = 0, LEAF = 1 };

	protected:
		DynAabbTreeInternalNode* parent;
		EDMath::Aabb AABB;
		DynAabbTreeNodeType dynAabbTreeNodeType;
		IOcclusionQueryInterface* occQueryInterface;

	public:
		DynAabbTreeNode(void);
		virtual ~DynAabbTreeNode()
		{
			delete occQueryInterface;
			occQueryInterface = 0;		
		}

		inline DynAabbTreeNodeType GetDynAabbTreeNodeType(void){ return dynAabbTreeNodeType; }

		inline const EDMath::Aabb& GetAABB(void)const { return AABB; }

		//Aabb& GetAABB(void) { return AABB; } 

		const DynAabbTreeInternalNode* GetParentDynAabbTreeNode(void) const { return parent; }
	};

	class DynAabbTreeInternalNode : public DynAabbTreeNode
	{
		friend class DynAabbTree;
	private:
		DynAabbTreeNode* children[2];
	public:
		// What is this funk?  You would only be initializing the first one in the array anyways
		//#pragma warning( disable : 4351 )
		DynAabbTreeInternalNode(void) /*: children()*/ { dynAabbTreeNodeType = INTERNAL; }
		~DynAabbTreeInternalNode();

		int NodeSide(DynAabbTreeNode* node);
	};

	class DynAabbTreeLeafNode : public DynAabbTreeNode
	{
		friend class DynAabbTree;
		friend class GameObject;

		GameObject* gameObject;

		DynAabbTree* tree;
	public:
		DynAabbTreeLeafNode(GameObject* gameObject);
		virtual ~DynAabbTreeLeafNode(){}
	};
}