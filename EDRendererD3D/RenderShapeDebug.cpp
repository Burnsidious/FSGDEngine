#include "precompiled.h"
#include "RenderShapeDebug.h"
#include "ViewPortManager.h"
#include "Renderer.h"
#include "VertexBufferManager.h"


using namespace DirectX;

namespace EDRendererD3D
{
	RenderShapeDebug::RenderShapeDebug(void)
	{
		linesVertexBufferPtr = nullptr;
		linesPointCount = 0;
		trianglesVertexBufferPtr = nullptr;
		trianglesPointCount = 0;

		worldMatrix._12 = worldMatrix._13 = worldMatrix._14 = 
		worldMatrix._21 = worldMatrix._23 = worldMatrix._24 = 
		worldMatrix._31 = worldMatrix._32 = worldMatrix._34 = 
		worldMatrix._41 = worldMatrix._42 = worldMatrix._43 = 0;

		worldMatrix._11 = worldMatrix._22 = worldMatrix._33 = worldMatrix._44 = 1;
	}

	RenderShapeDebug::~RenderShapeDebug(void)
	{
	}

	void RenderShapeDebug::RenderDebugLines(RenderNode &node)
	{
		Renderer::theContextPtr->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		RenderShapeDebug &shape = (RenderShapeDebug &)node;
		RenderContext &context = *shape.GetRenderContext();

		UINT stride = sizeof(VERTEX_POSCOLOR);
		UINT offset = 0;
		Renderer::theContextPtr->IASetVertexBuffers(0, 1, &shape.linesVertexBufferPtr, &stride, &offset);

		// Set matrices
		XMMATRIX world = XMLoadFloat4x4(shape.GetWorldMatrixPtr());
		XMMATRIX viewProj  = XMLoadFloat4x4((XMFLOAT4X4 *)&ViewPortManager::GetReference().GetActiveViewProjection());
		XMMATRIX worldViewProj = world*viewProj;
		Renderer::SetPerObjectData(worldViewProj, world);

		Renderer::theContextPtr->Draw(shape.linesPointCount, 0);

		shape.linesPointCount = 0;
	}

	void RenderShapeDebug::RenderDebugTriangles(RenderNode &node)
	{
		Renderer::theContextPtr->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderShapeDebug &shape = (RenderShapeDebug &)node;
		RenderContext &context = *shape.GetRenderContext();

		UINT stride = sizeof(VERTEX_POSCOLOR);
		UINT offset = 0;
		Renderer::theContextPtr->IASetVertexBuffers(0, 1, &shape.trianglesVertexBufferPtr, &stride, &offset);

		// Set matrices
		XMMATRIX world = XMLoadFloat4x4(shape.GetWorldMatrixPtr());
		XMMATRIX viewProj  = XMLoadFloat4x4((XMFLOAT4X4 *)&ViewPortManager::GetReference().GetActiveViewProjection());
		XMMATRIX worldViewProj = world*viewProj;
		Renderer::SetPerObjectData(worldViewProj, world);

		Renderer::theContextPtr->Draw(shape.trianglesPointCount, 0);

		shape.trianglesPointCount = 0;
	}
}