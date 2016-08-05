#include "precompiled.h"
#include "Renderer.h"
#include "RenderShapeWithTexture.h"

using namespace EDRendererD3D;
using namespace DirectX;
using namespace EDUtilities;

void RenderShapeWithTexture::Initialize(std::vector< ContentHandle<RenderForm> >&formHandles,
    XMFLOAT4X4* localMatrixPtr)
{
	SetRenderFormHandle(formHandles);
	
	if( localMatrixPtr != 0 )
		SetWorldMatrix(*localMatrixPtr);

	RenderFunc = IndexedPrimitiveRenderFunc;
}

void RenderShapeWithTexture::SetTextures(RenderShapeWithTexture &shape)
{
    // Implement a solution for the Renderer Lab
    SetTexturesSolution(shape);
}

void RenderShapeWithTexture::IndexedPrimitiveRenderFunc(RenderNode &node)
{
    // Implement a solution for the Renderer Lab
    return WithTextureIndexedPrimitiveRenderFuncSolution(node);
}
