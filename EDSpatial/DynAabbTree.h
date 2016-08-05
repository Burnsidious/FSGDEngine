#pragma once

#include "../EDMemoryManager/OverloadNew.h"

#include "../EDMath/aabb.h"
using namespace EDMath;

#include <stack>

namespace EDSpatial
{
	template<typename T>
	class DynAabbTreeT;

	typedef void (*LeafDataOverlapFunc)(void* a, void* b, const Float3& a2bOffset, void* voidPtr);

	class DynAabbTree
	{
	public:
		class Branch;
		class Leaf;
		class Node;
	protected:
		DynAabbTree(void) : rootPtr(0), leafListHead(0) {}
		DynAabbTree(const DynAabbTree&){}
		DynAabbTree& operator=(const DynAabbTree&){ return *this; }

		virtual Branch* createBranch(void)const = 0;
		virtual Leaf* createLeaf(void)const = 0;

		static float cost(const Float3& centerA, const Float3& centerB);

		static void TreeTreeOverlap( Node* a, Node* b, const Float3& a2bOffset, LeafDataOverlapFunc overlapFunc, void* voidPtr );
		static void LeafTreeOverlap( EDSpatial::DynAabbTree::Leaf* leaf, EDSpatial::DynAabbTree::Node* node, const Float3& a2bOffset, LeafDataOverlapFunc overlapFunc, void* voidPtr ); 

		Node* rootPtr;
		Leaf* leafListHead;

		void Insert(Leaf* leaf);
		bool Remove(Leaf* leaf);
		void Update(Leaf* leaf);

		friend void buildAabbSolution( EDMath::Aabb& out, const EDMath::Aabb& a, const EDMath::Aabb& b );
		friend bool AabbsAreEqualSolution( const EDMath::Aabb& a, const EDMath::Aabb& b );
		friend bool AabbContainsAabbSolution( const EDMath::Aabb& container, const EDMath::Aabb& item );
		friend bool Aabb2AabbOffsetSolution(const Aabb& a, const Aabb& b, const Float3& a2bOffset);
		friend bool AabbAabbSolution(const Aabb& a, const Aabb& b);
		friend float costSolution(const Float3& centerA, const Float3& centerB);
		friend void InsertSolution(EDSpatial::DynAabbTree* tree, EDSpatial::DynAabbTree::Leaf* leaf);
		friend bool RemoveSolution(EDSpatial::DynAabbTree* tree, Leaf* leaf);
		friend void UpdateSolution(EDSpatial::DynAabbTree* tree, EDSpatial::DynAabbTree::Leaf* leaf);
		friend void TreeTreeOverlapSolution( EDSpatial::DynAabbTree::Node* a, EDSpatial::DynAabbTree::Node* b, const Float3& a2bOffset, LeafDataOverlapFunc overlapFunc, void* voidPtr );
		friend void LeafTreeOverlapSolution( EDSpatial::DynAabbTree::Leaf* leaf, EDSpatial::DynAabbTree::Node* node, const Float3& a2bOffset, LeafDataOverlapFunc overlapFunc, void* voidPtr );
	public:
		static void TreeTreeOverlap( DynAabbTree* treeA, DynAabbTree* treeB, const Float3& a2bOffset, LeafDataOverlapFunc overlapFunc, void* voidPtr = 0);
		static void SelfTreeOverlap( DynAabbTree* tree, LeafDataOverlapFunc overlapFunc, void* voidPtr = 0 );

		bool empty(void){ return (rootPtr == 0); }

		const Node* GetRoot(void)const{ return rootPtr; }
		Node* GetRoot(void){ return rootPtr; }

		virtual ~DynAabbTree(){}

		class Node
		{
		public:
			enum NodeType{ INVALID_TYPE = -1, BRANCH = 0, LEAF = 1 };
			enum ChildSide{ INVALID_CHILD = -1, LEFT = 0, RIGHT = 1 };

			Node(NodeType t = INVALID_TYPE) : type(t), parentPtr(0) {}

			virtual ~Node(){}
			
			NodeType GetType(void){ return type; }
			const EDMath::Aabb& GetAabb(void)const { return aabb; }

			virtual Branch* asBranch(void){ return 0; }
			virtual Leaf* asLeaf(void){ return 0; }

			NodeType type;
			EDMath::Aabb aabb;
			Branch* parentPtr;
		};

		class Branch : public Node
		{
		public:
			Branch(void) : Node(BRANCH), leftChild(0), rightChild(0) {}
			virtual ~Branch(){}

			union
			{
				Node* children[2];

				struct
				{
					Node* leftChild;
					Node* rightChild;
				};
			};

			ChildSide GetChildSide(const Node* n)
			{
				if( n == leftChild )
					return LEFT;
				if( n == rightChild )
					return RIGHT;

				return INVALID_CHILD;
			}


			Branch* asBranch(void){ return this; }

			Node* GetLeftChild(void){ return leftChild; }
			Node* GetRightChild(void){ return rightChild; }
		};

		class Leaf : public Node
		{
		public:
			Leaf(void) : Node(LEAF), tree(0), prevPtr(0), nextPtr(0) {}
			virtual ~Leaf(){}

			virtual EDMath::Aabb GetObjectAabb(void) = 0;
			virtual void* GetObjectPtr(void) = 0;

			EDMath::Aabb lastAabb;
			EDMath::Aabb currentAabb;
			DynAabbTree* tree;
			
			Leaf* prevPtr;
			Leaf* nextPtr;
		};

		template<typename T>
		class LeafT : public Leaf
		{
		public:
			LeafT(void){}
			virtual ~LeafT(){}

			LeafT* asLeaf(void){ return this; }

			EDMath::Aabb GetObjectAabb(void)
			{
				return T::GetAabb(object);
			}

			void* GetObjectPtr(void){ return &object; }

			T object;
		};

		template<typename T>
		class LeafT<T*> : public Leaf
		{
		public:
			LeafT(void){}
			virtual ~LeafT(){}

			LeafT* asLeaf(void){ return this; }

			EDMath::Aabb GetObjectAabb(void)
			{
				return T::GetAabb(*object);
			}

			void* GetObjectPtr(void){ return object; }

			T* object;
		};
		
		template<typename T>
		class traverser
		{
			void traverse(Node* node)
			{
				if( node == 0 )
					return;

				if( !testAabb( node->GetAabb() ) )
					return;

				if( LeafT<T>* leaf = (LeafT<T>*)node->asLeaf() )
				{
					onObject( leaf->object );
				}
				else if( Branch* branch = node->asBranch() )
				{
					traverse( branch->leftChild );
					traverse( branch->rightChild );
				}
			}

		public:
			virtual ~traverser(){}

			void traverse(DynAabbTreeT<T>* tree)
			{
				if( tree != 0 )
					traverse( tree->rootPtr );
			}

			virtual bool testAabb(const Aabb& aabb)const { return true; }

			virtual void onObject(T object) = 0;
		};
	};

	template<typename T>
	class DynAabbTreeT : public DynAabbTree
	{
	public:
		virtual ~DynAabbTreeT(){}

		LeafT<T>* Insert(T& object)
		{
			LeafT<T>* leaf = createLeaf();
			leaf->object = object;
			leaf->lastAabb = leaf->aabb = leaf->GetObjectAabb();
			
			DynAabbTree::Insert(leaf);

			return leaf;
		}

		bool Remove(LeafT<T>* leaf)
		{
			return DynAabbTree::Remove(leaf);
		}

		void Update(LeafT<T>* leaf)
		{
			DynAabbTree::Update(leaf);
		}

		Branch* createBranch(void)const 
		{ 
			return new Branch();
		}

		LeafT<T>* createLeaf(void)const
		{ 
			return new LeafT<T>();
		}

	protected:
		DynAabbTreeT(void){}
		DynAabbTreeT(const DynAabbTreeT&){}
		DynAabbTreeT& operator=(const DynAabbTreeT&){ return *this; }
	};
}