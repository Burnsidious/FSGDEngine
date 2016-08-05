#include "precompiled.h"
#include "Renderer.h"
#include "RenderShape.h"
#include "../EDRendererD3D/ViewPortManager.h"

#include "../EDMath/Float4x4.h"

using namespace EDRendererD3D;
using namespace EDMath;
using namespace DirectX;

void RenderShape::AddToContextSet(RenderContext *contextPtr)
{
	if( renderNumber < Renderer::GetRenderNumber() )
	{
		renderNumber = Renderer::GetRenderNumber();
		if(!contextPtr)
			GetRenderContext(currentLOD)->AddRenderNode(this);
		else
			contextPtr->AddRenderNode(this);
	}
}

void RenderShape::Initialize(std::vector< EDUtilities::ContentHandle<RenderForm> >&formHandles,
    XMFLOAT4X4 *localMatrixPtr)
{
	SetRenderFormHandle(formHandles);
	
	if( localMatrixPtr != 0 )
		worldMatrix = *localMatrixPtr;

	RenderFunc = IndexedPrimitiveRenderFunc;
}

const EDMath::Sphere& RenderShape::GetSphere(void)
{
	const EDMath::Sphere& meshSphere = GetRenderMesh()->GetSphere();
	TransformPoint( sphere.center, meshSphere.center, *((Float4x4 *)&worldMatrix) );
	sphere.radius = meshSphere.radius;

	return sphere;
}

const EDMath::Sphere& RenderShape::GetLocalSphere(void) const
{
	return GetRenderMesh()->GetSphere();
}

void RenderShape::IndexedPrimitiveRenderFunc(RenderNode &node)
{
    // Implement a solution for the Renderer Lab
    return IndexedPrimitiveRenderFuncSolution(node);
}
