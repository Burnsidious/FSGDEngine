#include "precompiled.h"
#include "Renderer.h"
#include "RenderShapeSkinned.h"

using namespace EDRendererD3D;
using namespace DirectX;
using namespace EDUtilities;

void RenderShapeSkinned::Initialize(std::vector< ContentHandle<RenderForm> >&formHandles,
    XMFLOAT4X4* localMatrixPtr)
{
	SetRenderFormHandle(formHandles);
	
	if( localMatrixPtr != 0 )
		SetWorldMatrix(*localMatrixPtr);

	RenderFunc = IndexedPrimitiveRenderFunc;
}

void RenderShapeSkinned::SetBoneMatrices(const std::vector<EDMath::Float4x4> &_boneMatrices)
{
	if(_boneMatrices.size() > boneMatrices.size())
		boneMatrices.resize(_boneMatrices.size());

	memcpy(&boneMatrices[0], &_boneMatrices[0], _boneMatrices.size() * sizeof(EDMath::Float4x4));
}

void RenderShapeSkinned::IndexedPrimitiveRenderFunc(RenderNode &node)
{
    // Implement a solution for the Renderer Lab
    return IndexedPrimitiveRenderFuncSolution(node);
}
