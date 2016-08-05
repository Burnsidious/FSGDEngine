#include "precompiled.h"

#include "../EDRendererD3D/RenderShape.h"
#include "../EDRendererD3D/RenderShapeSkinned.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/ViewPortManager.h"
#include "../EDRendererD3D/IndexBuffer.h"
#include "../EDRendererD3D/VertexBuffer.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/GraphicsProfiler.h"

#include "../EDUtilities/PrintConsole.h"
using EDUtilities::PrintConsole;
#include "../EDMath/Float3.h"
using EDMath::Float3;

using namespace DirectX;

namespace EDRendererD3D
{
	void RenderShape::IndexedPrimitiveRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderShape::IndexedPrimitiveRenderFuncSolution");
			added = true;
		}
		RenderShape &shape = (RenderShape &)node;
		RenderMesh &mesh = *shape.GetRenderMesh();
		//RenderContext &context = *shape.GetRenderContext();
		
		// Set matrices
		XMMATRIX world = XMLoadFloat4x4(shape.GetWorldMatrixPtr());
		XMMATRIX viewProj  = XMLoadFloat4x4(
			(XMFLOAT4X4 *)&ViewPortManager::GetReference().GetActiveViewProjection());
		XMMATRIX worldViewProj = world*viewProj;
		Renderer::SetPerObjectData(worldViewProj, world);

		Renderer::theContextPtr->DrawIndexed(mesh.GetPrimitiveCount() * 3, 
			mesh.GetStartIndex(), mesh.GetStartVertex());
	}

	void RenderShapeSkinned::IndexedPrimitiveRenderFuncSolution(RenderNode &node)
	{
		GraphicsProfiler::GetReference().BeginEvent(0, L"Skinned");
		RenderShapeSkinned &shape = (RenderShapeSkinned &)node;
		RenderMesh &mesh = *shape.GetRenderMesh();
		//RenderContext &context = *shape.GetRenderContext();
		
		SetTextures(shape);

		// Set matrices
		XMFLOAT4X4 tempViewProj = *(XMFLOAT4X4 *)&ViewPortManager::GetReference().GetActiveViewProjection();
		XMFLOAT4X4 tempWorld = *(XMFLOAT4X4 *)shape.GetWorldMatrixPtr();

		XMFLOAT4X4 bones[cbPerSkinnedObject::NUM_BONES];

		memcpy(&bones[0], &shape.boneMatrices[0], sizeof(XMFLOAT4X4) * shape.boneMatrices.size());

		Renderer::SetPerObjectData(tempViewProj, tempWorld, bones);

		Renderer::theContextPtr->DrawIndexed(mesh.GetPrimitiveCount() * 3, 
			mesh.GetStartIndex(), mesh.GetStartVertex());
		GraphicsProfiler::GetReference().EndEvent();
	}
}