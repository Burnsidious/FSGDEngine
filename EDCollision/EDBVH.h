#pragma once

#include "EDCollision.h"
#include "../EDMath/aabb.h"
#include "../EDMath/wingedTriangle.h"
#include "../EDMath/boundingVolume.h"

#ifdef _DEBUG
	#include <list>
	using namespace std;		
#endif

using namespace EDMath;

namespace EDCollision
{
	class EDBVH
	{
	public:
		class Node
		{
			friend class EDBVH;
		public:
			Node(void) : leftChild(0), rightChild(0) {}
			
			~Node();

			enum NodeType{ BRANCH = 0, LEAF };

			const Node* getLeftChild(void)const{ return leftChild; }
			const Node* getRightChild(void)const{ return rightChild; }
			const Aabb& getBounds(void)const{ return bounds; }
			size_t getStartIndex(void)const{ return startIndex; }
			size_t getTriCount(void)const{ return triCount; }

			NodeType getNodeType(void)const{ return nodeType; }
			
		private:				
			NodeType nodeType;
			Aabb bounds;
			union
			{
				struct
				{
					Node* leftChild;
					Node* rightChild;
				};

				struct
				{
					size_t startIndex;
					size_t triCount;
				};
			};

			friend EDBVH::Node* buildTreeSolution( EDBVH* bvh, unsigned int startIndex, unsigned int count );
		};

		bool Collide(const BoundingVolume& boundingVolume, unsigned int& triIndex, EDCollision::Contact* contact = 0);

	private:
		Node* rootPtr;

		Float3* vertices;
		WingedTriangle* triangles;
		unsigned int vertCount;
		unsigned int triCount;
		
		Float3* normals;

		static const unsigned int MIN_TRIS_PER_LEAF = 2;

		void computeBounds( Aabb& aabb, unsigned int startIndex, unsigned int count );
		
		unsigned int partitionTriangles( unsigned int startIndex, unsigned int count );

		Node* buildTree( unsigned int startIndex, unsigned int count );

#ifdef _DEBUG
		list< Node* > leafList;
#endif

	public:
		EDBVH(void);
		~EDBVH();

		const Node* getRoot(void) const { return rootPtr; }

		const WingedTriangle* getTriangles(void)const { return triangles; }
		unsigned int getTriangleCount(void)const { return triCount; }
		
		const Float3* getVertices(void)const { return vertices; }
		unsigned int getVertexCount(void)const { return vertCount; }

		const Float3* getTriangleNormals(void)const { return normals; }

		static EDBVH* BuildBVH(const Float3* verts, unsigned int vertCount, const unsigned int* tris, unsigned int triCount);

		static EDBVH* Load(const char* fileName);

		static void BVtoBVH(const BoundingVolume& boundingVolume, const Aabb& bvAabb, const EDBVH::Node* current, const EDBVH* bvh, EDCollision::Contact& contact, unsigned int& triIndex);

		friend void computeBoundsSolution( EDBVH* bvh, Aabb& aabb, unsigned int startIndex, unsigned int count );
		friend unsigned int partitionTrianglesSolution( EDBVH* bvh, unsigned int startIndex, unsigned int count );
		friend Node* buildTreeSolution( EDBVH* bvh, unsigned int startIndex, unsigned int count );
		friend EDBVH* BuildBVHSolution(const Float3* verts, unsigned int vertCount, const unsigned int* tris, unsigned int triCount);
		friend void BVtoBVHSolution(const BoundingVolume& boundingVolume, const Aabb& bvAabb, const EDBVH::Node* current, const EDBVH* bvh, EDCollision::Contact& contact, unsigned int& triIndex);
	};
}