#include "DynAabbTree.h"

namespace EDSpatial
{
	void buildAabb( EDMath::Aabb& out, const EDMath::Aabb& a, const EDMath::Aabb& b )
	{
		buildAabbSolution( out, a, b );
	}

	bool AabbsAreEqual( const EDMath::Aabb& a, const EDMath::Aabb& b )
	{
		return AabbsAreEqualSolution( a, b );
	}

	bool AabbContainsAabb( const EDMath::Aabb& container, const EDMath::Aabb& item )
	{
		return AabbContainsAabbSolution( container, item );
	}

	bool Aabb2AabbOffset(const Aabb& a, const Aabb& b, const Float3& a2bOffset)
	{
		return Aabb2AabbOffsetSolution( a, b, a2bOffset );
	}

	bool AabbAabb(const Aabb& a, const Aabb& b)
	{
		return AabbAabbSolution( a, b );
	}

	float DynAabbTree::cost(const Float3& centerA, const Float3& centerB)
	{
		return costSolution( centerA, centerB );
	}

	void DynAabbTree::Insert(Leaf* leaf)
	{
		InsertSolution(this, leaf);
	}

	bool DynAabbTree::Remove(Leaf* leaf)
	{
		return RemoveSolution(this, leaf);
	}

	void DynAabbTree::Update(Leaf* leaf)
	{
		UpdateSolution( this, leaf );
	}

	void DynAabbTree::TreeTreeOverlap( DynAabbTree* treeA, DynAabbTree* treeB, const Float3& a2bOffset, LeafDataOverlapFunc overlapFunc, void* voidPtr )
	{
		if( treeA->rootPtr != 0 && treeB->rootPtr != 0 )
		{
			if( treeA != treeB )
				TreeTreeOverlap( treeA->rootPtr, treeB->rootPtr, a2bOffset, overlapFunc, voidPtr );
			else
				SelfTreeOverlap( treeA, overlapFunc, voidPtr );
		}
	}

	void DynAabbTree::LeafTreeOverlap( EDSpatial::DynAabbTree::Leaf* leaf, EDSpatial::DynAabbTree::Node* node, const Float3& a2bOffset, LeafDataOverlapFunc overlapFunc, void* voidPtr )
	{
		LeafTreeOverlapSolution( leaf, node, a2bOffset, overlapFunc, voidPtr );
	}

	void DynAabbTree::TreeTreeOverlap( Node* a, Node* b, const Float3& a2bOffset, LeafDataOverlapFunc overlapFunc, void* voidPtr )
	{
		TreeTreeOverlapSolution( a, b, a2bOffset, overlapFunc, voidPtr );
	}

	void DynAabbTree::SelfTreeOverlap( DynAabbTree* tree, LeafDataOverlapFunc overlapFunc, void* voidPtr )
	{
		for( Leaf* currentLeaf = tree->leafListHead; currentLeaf != 0; currentLeaf = currentLeaf->nextPtr )
		{
			DynAabbTree::Node* currentNode = currentLeaf;
			auto parent = currentLeaf->parentPtr;

			while( parent != 0 )
			{
				if( parent->leftChild == currentNode )
					LeafTreeOverlap( currentLeaf, parent->rightChild, EDMath::ZERO_VECTOR, overlapFunc, voidPtr );

				currentNode = parent;
				parent = parent->parentPtr;
			}
		}
	}
}