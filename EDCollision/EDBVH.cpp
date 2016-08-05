#include "EDBVH.h"

#include "../EDMath/aabb.h"
#include "../EDUtilities/ContentManager.h"
#include "../EDUtilities/GDMeshStream.h"
#include "../EDCollision/aabb.h"

#include <assert.h>
#include <vector>

using namespace EDUtilities;
using namespace std;
using namespace EDMath;

namespace EDCollision
{
	EDBVH::Node::~Node()
	{
		if( nodeType == BRANCH )
		{
			delete leftChild;
			delete rightChild;
		}
	}

	EDBVH::EDBVH(void) : rootPtr(0), vertices(0),
		triangles(0), triCount(0), normals(0), vertCount(0) {}

	EDBVH::~EDBVH()
	{
		delete rootPtr;
		delete vertices;
		delete triangles;
		delete normals;
	}

	void EDBVH::computeBounds( EDMath::Aabb& aabb, unsigned int startIndex, unsigned int count )
	{
		computeBoundsSolution( this, aabb, startIndex, count );
	}
		
	unsigned int EDBVH::partitionTriangles( unsigned int startIndex, unsigned int count )
	{
		return partitionTrianglesSolution( this, startIndex, count );
	}

	EDBVH::Node* EDBVH::buildTree( unsigned int startIndex, unsigned int count )
	{
		return buildTreeSolution( this, startIndex, count );
	}

	EDBVH* EDBVH::BuildBVH(const Float3* verts, unsigned int vertCount, const unsigned int* tris, unsigned int triCount)
	{
		return BuildBVHSolution( verts, vertCount, tris, triCount );
	}

	EDBVH* EDBVH::Load(const char* pInFileName)
	{
		string temp = ContentManager::theContentPath;
		temp += pInFileName;
		const char* fileName = temp.c_str();

		ContentHandle<GDMeshStreamBinary> meshHandle = ContentManager::Load<GDMeshStreamBinary>(fileName);
			
		ContentHandle<GDAttribute<Point>> pointHandle = meshHandle.GetContent()->ReadAttribute<Point>();
		size_t numVerts = pointHandle.GetContent()->size();

		ContentHandle<GDAttribute<TriVertIndices>> indicesHandle = meshHandle.GetContent()->GetIndicesHandle();

		return BuildBVH( &pointHandle.GetContent()->operator[](0), (unsigned int)numVerts, indicesHandle.GetContent()->operator[](0).indices, (unsigned int)indicesHandle.GetContent()->size() );
	}

	void EDBVH::BVtoBVH(const BoundingVolume& boundingVolume, const Aabb& bvAabb, const EDBVH::Node* current, const EDBVH* bvh, EDCollision::Contact& contact, unsigned int& triIndex)
	{
		return BVtoBVHSolution( boundingVolume, bvAabb, current, bvh, contact, triIndex );
	}

	bool EDBVH::Collide(const BoundingVolume& boundingVolume, unsigned int& triIndex, EDCollision::Contact* contact)
	{
		Aabb bvAabb = boundingVolume.GetAabb();

		EDCollision::Contact newContact;
		newContact.numPoints = 0;
		newContact.PenetrationDepth = -FLT_MAX;

		BVtoBVH( boundingVolume, bvAabb, rootPtr, this, newContact, triIndex );

		if( newContact.PenetrationDepth > 0.0f )
		{
			if( contact )
				*contact = newContact;

			return true;
		}

		return false;
	}
}