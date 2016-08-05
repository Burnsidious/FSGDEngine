#include "precompiled.h"
#include "../EDUtilities/PrintConsole.h"

#include "../EDMath/aabb.h"
#include "../EDCollision/aabb.h"
#include "../EDCollision/EDBVH.h"

using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDCollision
{
	void computeBoundsSolution( EDBVH* bvh, EDMath::Aabb& aabb, unsigned int startIndex, unsigned int count )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "EDBVH", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::computeBoundsSolution");
			added = true;
		}

		Float3 vmin( FLT_MAX, FLT_MAX, FLT_MAX );
		Float3 vmax( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		
		unsigned int endIndex = startIndex + count;
		for(unsigned int triIndex = startIndex; triIndex < endIndex; ++triIndex)
		{
			WingedTriangle& tri = bvh->triangles[triIndex];
			
			for(unsigned int vertNum = 0; vertNum < 3; ++vertNum)
			{
				Float3& vert = bvh->vertices[ tri.indices[vertNum] ];

				for(unsigned int axis = 0; axis < 3; ++axis)
				{
					if( vmin.v[axis] > vert.v[axis] )
						vmin.v[axis] = vert.v[axis];

					if( vmax.v[axis] < vert.v[axis] )
						vmax.v[axis] = vert.v[axis];
				}
			}
		}

		aabb.center = (vmin + vmax) * 0.5f;
		aabb.extents = vmax - aabb.center;
	}

	unsigned int partitionTrianglesSolution( EDBVH* bvh, unsigned int startIndex, unsigned int count )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "EDBVH", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::partitionTrianglesSolution");
			added = true;
		}

		Float3 avgCentroid(0.0f, 0.0f, 0.0f);

		Float3 vmin( FLT_MAX, FLT_MAX, FLT_MAX );
		Float3 vmax( -FLT_MAX, -FLT_MAX, -FLT_MAX );

		unsigned int endIndex = startIndex + count;
		for(unsigned int triIndex = startIndex; triIndex < endIndex; ++triIndex)
		{
			WingedTriangle& tri = bvh->triangles[triIndex];
			
			Float3 centroid = ( bvh->vertices[ tri.i0 ] + bvh->vertices[ tri.i1 ] + bvh->vertices[ tri.i2 ] ) * (1.0f/3.0f); 

			for(unsigned int axis = 0; axis < 3; ++axis)
			{
				if( vmin.v[axis] > centroid.v[axis] )
					vmin.v[axis] = centroid.v[axis];
				if( vmax.v[axis] < centroid.v[axis] )
					vmax.v[axis] = centroid.v[axis];
			}

			avgCentroid += centroid;
		}

		avgCentroid *= (1.0f / count);

		Float3 delta = vmax - vmin;

		unsigned int splitAxis = 0;

		if( delta.y > delta.x )
			splitAxis = 1;
		if( delta.z > delta.v[splitAxis] )
			splitAxis = 2;

		vector< WingedTriangle > lessTris;
		vector< WingedTriangle > greaterTris;

		for(unsigned int triIndex = startIndex; triIndex < endIndex; ++triIndex)
		{
			WingedTriangle& tri = bvh->triangles[triIndex];
			
			float centroidValue = ( bvh->vertices[ tri.i0 ].v[splitAxis] + bvh->vertices[ tri.i1 ].v[splitAxis] + bvh->vertices[ tri.i2 ].v[splitAxis] ) * (1.0f/3.0f);

			if( centroidValue < avgCentroid.v[splitAxis] )
				lessTris.push_back( bvh->triangles[triIndex] );
			else
				greaterTris.push_back( bvh->triangles[triIndex] );
		}

		unsigned int lessSize = (unsigned int)lessTris.size();
		unsigned int greaterSize = (unsigned int)greaterTris.size();

		memcpy_s( &bvh->triangles[startIndex], sizeof(WingedTriangle) * count, &lessTris[0], sizeof(WingedTriangle) * lessSize );

		unsigned int offset = startIndex + lessSize;
		unsigned int diff = count - lessSize;

		memcpy_s( &bvh->triangles[offset], sizeof(WingedTriangle) * diff, &greaterTris[0], sizeof(WingedTriangle) * greaterSize );

		return lessSize;
	}

	EDBVH::Node* buildTreeSolution( EDBVH* bvh, unsigned int startIndex, unsigned int count )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "EDBVH", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::buildTreeSolution");
			added = true;
		}

		assert( count > 0 );
		
		EDBVH::Node* newNode = new EDBVH::Node();

		if( count <= EDBVH::MIN_TRIS_PER_LEAF )
		{
			newNode->nodeType = EDBVH::Node::LEAF;
			newNode->startIndex = startIndex;
			newNode->triCount = count;
			
#ifdef _DEBUG
			bvh->leafList.push_back( newNode );
#endif
		}
		else
		{
			unsigned int partitionSize = partitionTrianglesSolution( bvh, startIndex, count );

			EDBVH::Node* left = buildTreeSolution( bvh, startIndex, partitionSize );
			EDBVH::Node* right = buildTreeSolution( bvh, startIndex+partitionSize, count - partitionSize );

			newNode->nodeType = EDBVH::Node::BRANCH;
			newNode->leftChild = left;
			newNode->rightChild = right;
		}

		computeBoundsSolution( bvh, newNode->bounds, startIndex, count );

		return newNode;
	}

	EDBVH* BuildBVHSolution(const Float3* verts, unsigned int vertCount, const unsigned int* tris, unsigned int triCount)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "EDBVH", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::BuildBVHSolution");
			added = true;
		}

		assert( triCount > 0 );

		EDBVH* bvh = new EDBVH();

		bvh->vertices = new Float3[vertCount];
		bvh->triangles = new WingedTriangle[triCount];
		bvh->normals = new Float3[triCount];
		bvh->triCount = triCount;
		bvh->vertCount = vertCount;

		for(unsigned int i = 0; i < triCount; ++i)
		{
			const unsigned int* indices = &tris[ i * 3 ];
			const Float3& v0 = verts[ indices[0] ];
			const Float3& v1 = verts[ indices[1] ];
			const Float3& v2 = verts[ indices[2] ];

			WingedTriangle& wingedTri = bvh->triangles[i];
			wingedTri.i0 = indices[0];
			wingedTri.i1 = indices[1];
			wingedTri.i2 = indices[2];

			bvh->vertices[ wingedTri.i0 ] = v0;
			bvh->vertices[ wingedTri.i1 ] = v1;
			bvh->vertices[ wingedTri.i2 ] = v2;
		}

		bvh->rootPtr = bvh->buildTree( 0, bvh->triCount );

		for(unsigned int i = 0; i < triCount; ++i)
		{
			const WingedTriangle& wingedTri = bvh->triangles[i];
			const Float3& v0 = verts[ wingedTri.i0 ];
			const Float3& v1 = verts[ wingedTri.i1 ];
			const Float3& v2 = verts[ wingedTri.i2 ];

			CrossProduct( bvh->normals[i], (v1 - v0), (v2 - v1) );
			bvh->normals[i].normalize();
		}

		WingedTriangle::ComputeAdjacencies( bvh->triangles, triCount );

		return bvh;
	}

	void BVtoBVHSolution(const BoundingVolume& boundingVolume, const Aabb& bvAabb, const EDBVH::Node* current, const EDBVH* bvh, EDCollision::Contact& contact, unsigned int& triIndex)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "EDBVH", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::BVtoBVHSolution");
			added = true;
		}

		if( !EDCollision::AabbToAabbSolution( bvAabb, current->getBounds() ) )
			return;

		if( current->getNodeType() == EDBVH::Node::BRANCH )
		{
			BVtoBVHSolution( boundingVolume, bvAabb, current->getLeftChild(), bvh, contact, triIndex );
			BVtoBVHSolution( boundingVolume, bvAabb, current->getRightChild(), bvh, contact, triIndex );
		}
		else
		{
			unsigned int startIndex = (unsigned int)current->getStartIndex();
			unsigned int endIndex = (unsigned int)current->getStartIndex() + (unsigned int)current->getTriCount();

			const EDMath::WingedTriangle* tris = bvh->getTriangles();
			const Float3* verts = bvh->getVertices();

			EDCollision::Contact newContact;

			for(unsigned int index = startIndex; index != endIndex; ++index)
			{
				const EDMath::WingedTriangle& tri = tris[index];
					
				if( EDCollision::CollideTriangle( boundingVolume, verts[ tri.i0 ], verts[ tri.i1 ], verts[ tri.i2 ], &newContact ) )
				{
					if( newContact.PenetrationDepth > contact.PenetrationDepth )
					{
						triIndex = index;
						contact = newContact;
					}
				}
			}
		}
	}
}