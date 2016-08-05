#include "AabbBvhLeafNode.h"
using namespace EDMath;

AabbBvhLeafNode::AabbBvhLeafData::AabbBvhLeafData(unsigned int* _triangleIndicesPtr, unsigned int _numTriangleIndices, const EDCollision::Aabb& _aabb)
{
	aabb = _aabb;
	containingLeafPtr = 0;
	vertexIndicesPtr = 0;

	triangleIndicesPtr = new unsigned int[_numTriangleIndices];

	numTriangleIndices = _numTriangleIndices;
	memcpy_s( triangleIndicesPtr, numTriangleIndices*sizeof(unsigned int), _triangleIndicesPtr, numTriangleIndices*sizeof(unsigned int) );
}

AabbBvhLeafNode::AabbBvhLeafData::AabbBvhLeafData(unsigned int* _triangleIndicesPtr, unsigned int _numTriangleIndices, unsigned int* _vertIndicesPtr, const EDCollision::Aabb& _aabb)
{
	aabb = _aabb;
	containingLeafPtr = 0;
	vertexIndicesPtr = 0;

	triangleIndicesPtr = new unsigned int[_numTriangleIndices];

	numTriangleIndices = _numTriangleIndices;
	memcpy_s( triangleIndicesPtr, numTriangleIndices*sizeof(unsigned int), _triangleIndicesPtr, numTriangleIndices*sizeof(unsigned int) );

	vertexIndicesPtr = new unsigned int[numTriangleIndices*3];
	memcpy_s( vertexIndicesPtr, numTriangleIndices*sizeof(unsigned int)*3, _vertIndicesPtr, numTriangleIndices*sizeof(unsigned int)*3);
}

AabbBvhLeafNode::AabbBvhLeafData::~AabbBvhLeafData(void)
{
	delete [] triangleIndicesPtr;
	delete [] vertexIndicesPtr;
}

bool AabbBvhLeafNode::AabbBvhLeafData::SplitData(EDTreeInternalNodeData*& splitter, EDTreeLeafNodeData*& resultPtr, unsigned int& newDataCount)
{
	AabbBvh* pBvh = (AabbBvh* )this->GetContainingLeaf()->GetOwnerTree();
	SourceData* sourceDataPtr = (SourceData*)pBvh->GetSourceData();

	if( this->GetNumTriangleIndices() <= sourceDataPtr->maxLeafSize )
	{
		CalculateVertexIndices();		
		return false;
	}

	BvhAabb* bvhAabbPtr = new BvhAabb;
	AabbBvhLeafData* newLeafDataPtr = new AabbBvhLeafData;

	bvhAabbPtr->max = this->GetBounds().max;
	bvhAabbPtr->min = this->GetBounds().min;
	splitter = bvhAabbPtr;

	resultPtr = newLeafDataPtr;
	newDataCount = 1;

	EDCollision::Aabb centAabb( Float3(FLT_MAX, FLT_MAX, FLT_MAX), Float3(-FLT_MAX, -FLT_MAX, -FLT_MAX) );
	newLeafDataPtr->aabb = centAabb;
	this->aabb = centAabb;

	for(unsigned int centNum = 0; centNum < this->GetNumTriangleIndices(); ++centNum)
	{
		const unsigned int& triIndex = this->GetTriangleIndices()[centNum];
		const Float3& centroid = sourceDataPtr->centroidsPtr[triIndex];

		for(unsigned int axis = 0; axis < 3; ++axis)
		{
			if( centroid.v[axis] > centAabb.max.v[axis] )
				centAabb.max.v[axis] = centroid.v[axis];
			if( centroid.v[axis] < centAabb.min.v[axis] )
				centAabb.min.v[axis] = centroid.v[axis];
		}
	}

	unsigned int splitAxis = 0;
	if( centAabb.max.v[splitAxis] - centAabb.min.v[splitAxis] <  centAabb.max.v[1] - centAabb.min.v[1] )
		splitAxis = 1;
	if( centAabb.max.v[splitAxis] - centAabb.min.v[splitAxis] <  centAabb.max.v[2] - centAabb.min.v[2] )
		splitAxis = 2;

	float mean = (centAabb.max.v[splitAxis] + centAabb.min.v[splitAxis]) * 0.5f;

	unsigned int* triIndexListLesser = new unsigned int[this->GetNumTriangleIndices()];
	unsigned int* triIndexListGreater = new unsigned int[this->GetNumTriangleIndices()];

	unsigned int lesserCount = 0;
	unsigned int greaterCount = 0;

	for(unsigned int centNum = 0; centNum < this->GetNumTriangleIndices(); ++centNum)
	{
		const unsigned int& triIndex = this->GetTriangleIndices()[centNum];
		const Float3& centroid = sourceDataPtr->centroidsPtr[triIndex];
		
		EDCollision::Aabb* targetAabb; 

		if( centroid.v[splitAxis] < mean )
		{
			targetAabb = &this->aabb;
			triIndexListLesser[lesserCount] = triIndex;
			++lesserCount;
		}
		else
		{
			targetAabb = &newLeafDataPtr->aabb;
			triIndexListGreater[greaterCount] = triIndex;
			++greaterCount;
		}

		for(unsigned int vertNum = 0; vertNum < 3; ++vertNum)
		{
			const Float3& vert = sourceDataPtr->pointsPtr[ sourceDataPtr->indiciesPtr[ triIndex*3 + vertNum ] ];
			for(unsigned int axis = 0; axis < 3; ++axis)
			{
				if( vert.v[axis] < targetAabb->min.v[axis] )
					targetAabb->min.v[axis] = vert.v[axis];

				if( vert.v[axis] > targetAabb->max.v[axis] )
					targetAabb->max.v[axis] = vert.v[axis];
			}
		}
	}

	this->numTriangleIndices = lesserCount;
	newLeafDataPtr->numTriangleIndices = greaterCount;

	delete [] this->triangleIndicesPtr;
	this->triangleIndicesPtr = new unsigned int[lesserCount];
	memcpy_s( this->triangleIndicesPtr, sizeof(unsigned int)*lesserCount, triIndexListLesser, sizeof(unsigned int)*lesserCount );
	delete [] triIndexListLesser;

	newLeafDataPtr->triangleIndicesPtr = new unsigned int[greaterCount];
	memcpy_s( newLeafDataPtr->triangleIndicesPtr, sizeof(unsigned int)*greaterCount, triIndexListGreater, sizeof(unsigned int)*greaterCount );
	delete [] triIndexListGreater;

	return true;
}

AabbBvhLeafNode* AabbBvhLeafNode::AabbBvhLeafData::CreateLeafNode(EDTree *ownerTreePtr)
{
	if( containingLeafPtr == 0 )
	{
		containingLeafPtr = new AabbBvhLeafNode;

		containingLeafPtr->SetOwnerTree(ownerTreePtr);
		containingLeafPtr->SetLeafNodeData(this);
	}

	return containingLeafPtr;
}

AabbBvhLeafNode* AabbBvhLeafNode::AabbBvhLeafData::GetContainingLeaf(void) const
{
	return containingLeafPtr;
}

void AabbBvhLeafNode::AabbBvhLeafData::CalculateVertexIndices(void)
{
	delete []vertexIndicesPtr;
	vertexIndicesPtr = new unsigned int[numTriangleIndices*3];

	const SourceData* sourceDataPtr = (const SourceData*)((AabbBvh*)GetContainingLeaf()->GetOwnerTree())->GetSourceData();

	for(unsigned int i = 0; i < numTriangleIndices; ++i)
	{
		vertexIndicesPtr[ i*3 ] = sourceDataPtr->indiciesPtr[ triangleIndicesPtr[i] * 3 ];
		vertexIndicesPtr[ i*3 + 1] = sourceDataPtr->indiciesPtr[ triangleIndicesPtr[i] * 3 + 1 ];
		vertexIndicesPtr[ i*3 + 2] = sourceDataPtr->indiciesPtr[ triangleIndicesPtr[i] * 3 + 2 ];
	}
}

const unsigned int* AabbBvhLeafNode::AabbBvhLeafData::GetVertexIndices(void) const
{
	return vertexIndicesPtr;
}

void AabbBvhLeafNode::Traverse( void* traversalDataPtr, void (*TraversalFunc)(const EDTreeNode*, void* traversalDataPtr, void* traversalFuncResultPtr ) ) const
{
	unsigned int testResult = 0;
	TraversalFunc( this, traversalDataPtr, &testResult );
}


