#include "precompiled.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDSpatial/DynAabbTree.h"

using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDSpatial
{
	void buildAabbSolution( EDMath::Aabb& out, const EDMath::Aabb& a, const EDMath::Aabb& b )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::buildAabbSolution");
			added = true;
		}

		Float3 min1 = a.Min();
		Float3 max1 = a.Max();

		Float3 min2 = b.Min();
		Float3 max2 = b.Max();

		for(int i = 0; i < 3; ++i)
		{
			if( min1.v[i] > min2.v[i] )
				min1.v[i] = min2.v[i];
			if( max1.v[i] < max2.v[i] )
				max1.v[i] = max2.v[i];
		}

		out.center = (min1 + max1) * 0.5f;
		out.extents = max1 - out.center;
	}

	bool AabbsAreEqualSolution( const EDMath::Aabb& a, const EDMath::Aabb& b )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::AabbsAreEqualSolution");
			added = true;
		}

		return ( a.center.x == b.center.x &&
				a.center.y == b.center.y &&
				a.center.z == b.center.z && 
				a.extents.x == b.extents.x &&
				a.extents.y == b.extents.y &&
				a.extents.z == b.extents.z );
	}

	bool AabbContainsAabbSolution( const EDMath::Aabb& container, const EDMath::Aabb& item )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::AabbContainsAabbSolution");
			added = true;
		}

		Float3 t = item.center - container.center;

		if( abs(t.x) + item.extents.x > container.extents.x )
			return false;

		if( abs(t.y) + item.extents.y > container.extents.y )
			return false;

		if( abs(t.z) + item.extents.z > container.extents.z )
			return false;

		return true;
	}

	bool Aabb2AabbOffsetSolution(const Aabb& a, const Aabb& b, const Float3& a2bOffset)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::Aabb2AabbOffsetSolution");
			added = true;
		}

		for(int i = 0; i < 3; ++i)
			if( abs(a.center.v[i] - (b.center.v[i] + a2bOffset.v[i]))  > a.extents.v[i] + b.extents.v[i] )
				return false;
		
		return true;
	}

	bool AabbAabbSolution(const Aabb& a, const Aabb& b)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::AabbAabbSolution");
			added = true;
		}

		if( abs( a.center.x - b.center.x ) > a.extents.x + b.extents.x )
			return false;
		if( abs( a.center.y - b.center.y ) > a.extents.y + b.extents.y )
			return false;
		if( abs( a.center.z - b.center.z ) > a.extents.z + b.extents.z )
			return false;

		return true;
	}

	float costSolution(const Float3& centerA, const Float3& centerB)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::costSolution");
			added = true;
		}

		return abs(centerA.x - centerB.x) + abs(centerA.y - centerB.y) + abs(centerA.z - centerB.z);
	}

	void InsertSolution(EDSpatial::DynAabbTree* tree, EDSpatial::DynAabbTree::Leaf* leaf)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::InsertSolution");
			added = true;
		}

		if( leaf->tree != 0 )
			return;

		leaf->tree = tree;

		if( tree->rootPtr == 0 )
		{
			tree->rootPtr = leaf;
			tree->leafListHead = leaf;
			leaf->prevPtr = 0;
			leaf->nextPtr = 0;

			return;
		}

		EDSpatial::DynAabbTree::Node* currentNode = tree->rootPtr;
		EDSpatial::DynAabbTree::Branch* branch = 0;
		while( (branch = currentNode->asBranch()) )
		{
			Aabb newBox;
			buildAabbSolution( newBox, branch->aabb, leaf->aabb );

			branch->aabb = newBox;

			float cost1 = costSolution( branch->children[0]->aabb.center, leaf->aabb.center );
			float cost2 = costSolution( branch->children[1]->aabb.center, leaf->aabb.center );

			if( cost1 < cost2 )
				currentNode = branch->children[0];
			else
				currentNode = branch->children[1];
		}

		EDSpatial::DynAabbTree::Branch* newBranch = tree->createBranch();
		
		buildAabbSolution( newBranch->aabb, currentNode->aabb, leaf->aabb );

		newBranch->children[0] = leaf;
		newBranch->children[1] = currentNode;

		if( currentNode->parentPtr != 0 )
			currentNode->parentPtr->children[ currentNode->parentPtr->GetChildSide( currentNode ) ] = newBranch;
		else
			tree->rootPtr = newBranch;

		newBranch->parentPtr = currentNode->parentPtr;
		currentNode->parentPtr = newBranch;
		leaf->parentPtr = newBranch;
		leaf->nextPtr = tree->leafListHead;
		leaf->prevPtr = 0;

		if( tree->leafListHead )
			tree->leafListHead->prevPtr = leaf;

		tree->leafListHead = leaf;
	}

	bool RemoveSolution(EDSpatial::DynAabbTree* tree, EDSpatial::DynAabbTree::Leaf* leaf)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::RemoveSolution");
			added = true;
		}

		if( leaf->tree != tree )
			return false;

		if( tree->rootPtr == leaf )
		{
			delete leaf;
			tree->rootPtr = 0;
			tree->leafListHead = 0;
			return true;
		}

		EDSpatial::DynAabbTree::Branch* parent = leaf->parentPtr;
		EDSpatial::DynAabbTree::Branch* grandParent = parent->parentPtr;
		EDSpatial::DynAabbTree::Node* sibling = (parent->children[0] == leaf ? parent->children[1] : parent->children[0]);

		if( grandParent == 0 )
		{
			leaf->parentPtr = 0;
			sibling->parentPtr = 0;
			parent->children[0] = parent->children[1] = 0;

			if( leaf->prevPtr != 0 )
				leaf->prevPtr->nextPtr = leaf->nextPtr;
			else
				tree->leafListHead = leaf->nextPtr;

			if( leaf->nextPtr != 0 )
				leaf->nextPtr->prevPtr = leaf->prevPtr;

			delete parent;
			delete leaf;
			tree->rootPtr = sibling;
			return true;
		}

		grandParent->children[ grandParent->GetChildSide(parent) ] = sibling;
		sibling->parentPtr = grandParent;

		parent->parentPtr = 0;
		parent->children[0] = parent->children[1] = 0;
		leaf->parentPtr = 0;

		if( leaf->prevPtr != 0 )
			leaf->prevPtr->nextPtr = leaf->nextPtr;
		else
			tree->leafListHead = leaf->nextPtr;

		if( leaf->nextPtr != 0 )
			leaf->nextPtr->prevPtr = leaf->prevPtr;

		delete parent;
		delete leaf;

		while(grandParent != 0)
		{
			Aabb newAabb;
			buildAabbSolution( newAabb, grandParent->children[0]->aabb, grandParent->children[1]->aabb );

			if( AabbsAreEqualSolution( newAabb, grandParent->aabb ) )
				break;

			grandParent->aabb = newAabb;

			grandParent = grandParent->parentPtr;
		}

		return true;
	}

	void UpdateSolution(EDSpatial::DynAabbTree* tree, EDSpatial::DynAabbTree::Leaf* leaf)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::UpdateSolution");
			added = true;
		}

		leaf->lastAabb = leaf->currentAabb;
		leaf->currentAabb = leaf->GetObjectAabb();

		Aabb newAabb;
		buildAabbSolution( newAabb, leaf->lastAabb, leaf->currentAabb );

		if( AabbContainsAabbSolution( leaf->aabb, newAabb ) )
			return;

		const float margin = 0.01f;
		newAabb.extents.x += margin;
		newAabb.extents.y += margin;
		newAabb.extents.z += margin;
		
		leaf->aabb = newAabb;

		if( tree->rootPtr == leaf )
			return;

		EDSpatial::DynAabbTree::Branch* parent = leaf->parentPtr;
		EDSpatial::DynAabbTree::Branch* grandParent = parent->parentPtr;
		EDSpatial::DynAabbTree::Node* sibling = (parent->children[0] == leaf ? parent->children[1] : parent->children[0]);
		
		if( grandParent != 0 )
			grandParent->children[ grandParent->GetChildSide(parent) ] = sibling;
		else
			tree->rootPtr = sibling;

		sibling->parentPtr = grandParent;

		while(grandParent != 0)
		{
			Aabb newAabb;
			buildAabbSolution( newAabb, grandParent->children[0]->aabb, grandParent->children[1]->aabb );

			if( AabbsAreEqualSolution( newAabb, grandParent->aabb ) )
				break;

			grandParent->aabb = newAabb;

			grandParent = grandParent->parentPtr;
		}

		EDSpatial::DynAabbTree::Node* currentNode = (grandParent != 0 ? grandParent : tree->rootPtr);
		EDSpatial::DynAabbTree::Branch* branch = 0;
		while( (branch = currentNode->asBranch()) )
		{
			Aabb newBox;
			buildAabbSolution( newBox, branch->aabb, leaf->aabb );

			branch->aabb = newBox;

			float cost1 = costSolution( branch->children[0]->aabb.center, leaf->aabb.center );
			float cost2 = costSolution( branch->children[1]->aabb.center, leaf->aabb.center );

			if( cost1 < cost2 )
				currentNode = branch->children[0];
			else
				currentNode = branch->children[1];
		}
		
		if( currentNode->parentPtr != 0 )
			currentNode->parentPtr->children[ currentNode->parentPtr->GetChildSide(currentNode) ] = leaf->parentPtr;
		else
			tree->rootPtr = leaf->parentPtr;

		leaf->parentPtr->parentPtr = currentNode->parentPtr;
		currentNode->parentPtr = leaf->parentPtr;

		if( leaf->parentPtr->children[0] == leaf )
			leaf->parentPtr->children[1] = currentNode;
		else
			leaf->parentPtr->children[0] = currentNode;

		buildAabbSolution( leaf->parentPtr->aabb, currentNode->aabb, leaf->aabb );
	}

	void LeafTreeOverlapSolution( EDSpatial::DynAabbTree::Leaf* leaf, EDSpatial::DynAabbTree::Node* node, const Float3& a2bOffset, LeafDataOverlapFunc overlapFunc, void* voidPtr )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::TreeTreeOverlapSolution");
			added = true;
		}

		EDSpatial::DynAabbTree::Leaf* leafPtrB = node->asLeaf();
		EDSpatial::DynAabbTree::Branch* branchPtrB = node->asBranch();

		if( leafPtrB != 0 )
		{
			if( !AabbAabbSolution( leaf->aabb, node->aabb ) )
				return;

			overlapFunc( leaf->GetObjectPtr(), leafPtrB->GetObjectPtr(), a2bOffset, voidPtr );
		}
		else
		{
			if( !AabbAabbSolution( leaf->aabb, node->aabb ) )
				return;

			EDSpatial::DynAabbTree::LeafTreeOverlap( leaf, branchPtrB->leftChild, a2bOffset, overlapFunc, voidPtr );
			EDSpatial::DynAabbTree::LeafTreeOverlap( leaf, branchPtrB->rightChild, a2bOffset, overlapFunc, voidPtr );
		}
	}

	void TreeTreeOverlapSolution( EDSpatial::DynAabbTree::Node* a, EDSpatial::DynAabbTree::Node* b, const Float3& a2bOffset, LeafDataOverlapFunc overlapFunc, void* voidPtr )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "DynAabbTree", Float3(1.0f, 0.0f, 0.0f), 
				L"DynAabbTree::TreeTreeOverlapSolution");
			added = true;
		}

		if( !AabbAabbSolution( a->aabb, b->aabb ) )
			return;

		EDSpatial::DynAabbTree::Leaf* leafPtrA = a->asLeaf();
		EDSpatial::DynAabbTree::Branch* branchPtrA = a->asBranch();

		EDSpatial::DynAabbTree::Leaf* leafPtrB = b->asLeaf();
		EDSpatial::DynAabbTree::Branch* branchPtrB = b->asBranch();
		
		if( leafPtrA != 0 && leafPtrB != 0 )
		{
			overlapFunc( leafPtrA->GetObjectPtr(), leafPtrB->GetObjectPtr(), a2bOffset, voidPtr );
		}
		else
		{
			float halfVolA = a->aabb.extents.x * a->aabb.extents.y * a->aabb.extents.z;
			float halfVolB = b->aabb.extents.x * b->aabb.extents.y * b->aabb.extents.z;

			if( leafPtrB != 0 || ( branchPtrA != 0 && halfVolA >= halfVolB ) )
			{
				EDSpatial::DynAabbTree::TreeTreeOverlap( branchPtrA->children[0], b, a2bOffset, overlapFunc, voidPtr );
				EDSpatial::DynAabbTree::TreeTreeOverlap( branchPtrA->children[1], b, a2bOffset, overlapFunc, voidPtr );
			}
			else
			{
				EDSpatial::DynAabbTree::TreeTreeOverlap( a, branchPtrB->children[0], a2bOffset, overlapFunc, voidPtr );
				EDSpatial::DynAabbTree::TreeTreeOverlap( a, branchPtrB->children[1], a2bOffset, overlapFunc, voidPtr );				
			}
		}
	}
}