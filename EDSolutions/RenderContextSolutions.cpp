#include "precompiled.h"

//#include "RenderContextSolutions.h"
#include "../EDRendererD3D/RenderContext.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/IndexBuffer.h"
#include "../EDRendererD3D/BlendStateManager.h"
#include "../EDRendererD3D/RasterizerStateManager.h"
#include "../EDUtilities/PrintConsole.h"

#include "../EDUtilities/InternalOutput.h"
using namespace EDUtilities;


using std::ifstream;
using namespace EDMath;

namespace EDRendererD3D
{
	void RenderContext::ContextSharedRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderContext::ContextSharedRenderFuncSolution");
			added = true;
		}

		RenderContext &context = (RenderContext &)node;
		Renderer::theContextPtr->IASetIndexBuffer(
			IndexBuffer::GetReference().GetIndices(), DXGI_FORMAT_R32_UINT, 0);

		Renderer::theContextPtr->VSSetShader( 
			context.GetRenderStageTechnique()->GetPass(0)->GetVertexShader(), NULL, 0 );
		Renderer::theContextPtr->PSSetShader( 
			context.GetRenderStageTechnique()->GetPass(0)->GetPixelShader(), NULL, 0 );
		Renderer::theContextPtr->GSSetShader( 
			context.GetRenderStageTechnique()->GetPass(0)->GetGeometryShader(), NULL, 0 );
		Renderer::theContextPtr->HSSetShader( 
			context.GetRenderStageTechnique()->GetPass(0)->GetHullShader(), NULL, 0 );
		Renderer::theContextPtr->DSSetShader( 
			context.GetRenderStageTechnique()->GetPass(0)->GetDomainShader(), NULL, 0 );
		Renderer::Render(context.GetRenderSet());
	}

	void RenderContext::ContextPosNormalUVRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderContext::ContextPosNormalUVRenderFuncSolution");
			added = true;
		}

		RenderContext &context = (RenderContext &)node;
		// Set the Vertex Layout
		Renderer::theContextPtr->IASetInputLayout(
			InputLayoutManager::GetReference().GetInputLayout(eVERTEX_POSNORMTEX));
		// Set the primitive type... Maybe this would be better in RenderShape so it can be specified by the mesh
		Renderer::theContextPtr->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(VERTEX_POSNORMTEX);
		UINT offset = 0;
		ID3D11Buffer *const vertexBufferPtr = VertexBufferManager::GetReference().GetPosNormTexBuffer().GetVertexBuffer();
		Renderer::theContextPtr->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);

		ContextSharedRenderFunc(node);
	}

	void RenderContext::ContextPosNormalTangentUVRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderContext::ContextPosNormalTangentUVRenderFunc");
			added = true;
		}
		
		RenderContext &context = (RenderContext &)node;
		// Set the Vertex Layout
		Renderer::theContextPtr->IASetInputLayout(
			InputLayoutManager::GetReference().GetInputLayout(eVERTEX_POSNORMTANTEX));
		// Set the primitive type... Maybe this would be better in RenderShape so it can be specified by the mesh
		Renderer::theContextPtr->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(VERTEX_POSNORMTANTEX);
		UINT offset = 0;
		ID3D11Buffer *const vertexBufferPtr = 
			VertexBufferManager::GetReference().GetPosNormTanTexBuffer().GetVertexBuffer();
		Renderer::theContextPtr->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);

		ContextSharedRenderFunc(node);
	}

	void RenderContext::ContextPositionRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderContext::ContextPositionRenderFuncSolution");
			added = true;
		}

		//ContextSharedRenderFunc(node);
		RenderContext &context = (RenderContext &)node;
		// Set the Vertex Layout
		Renderer::theContextPtr->IASetInputLayout(
			InputLayoutManager::GetReference().GetInputLayout(eVERTEX_POS));
		// Set the primitive type... Maybe this would be better in RenderShape so it can be specified by the mesh
		Renderer::theContextPtr->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(VERTEX_POS);
		UINT offset = 0;
		ID3D11Buffer *const vertexBufferPtr = VertexBufferManager::GetReference().GetPositionBuffer().GetVertexBuffer();
		Renderer::theContextPtr->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);

		ContextSharedRenderFunc(node);
	}

	void RenderContext::ContextPositionUVRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderContext::ContextPositionUVRenderFuncSolution");
			added = true;
		}

		//ContextSharedRenderFunc(node);
		RenderContext &context = (RenderContext &)node;
		// Set the Vertex Layout
		Renderer::theContextPtr->IASetInputLayout(
			InputLayoutManager::GetReference().GetInputLayout(eVERTEX_POSTEX));
		// Set the primitive type... Maybe this would be better in RenderShape so it can be specified by the mesh
		Renderer::theContextPtr->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(VERTEX_POSTEX);
		UINT offset = 0;
		ID3D11Buffer *const vertexBufferPtr = VertexBufferManager::GetReference().GetPositionTexBuffer().GetVertexBuffer();
		Renderer::theContextPtr->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);

		ContextSharedRenderFunc(node);
	}

	void RenderContext::ContextPositionColorRenderFuncSolution(RenderNode &node)
	{
		//assert(0);
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderContext::ContextPositionColorRenderFuncSolution");
			added = true;
		}
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Alpha);
		RenderContext &context = (RenderContext &)node;
		// Set the Vertex Layout
		Renderer::theContextPtr->IASetInputLayout(
			InputLayoutManager::GetReference().GetInputLayout(eVERTEX_POSCOLOR));
		// Set the primitive type... Maybe this would be better in RenderShape so it can be specified by the mesh
		Renderer::theContextPtr->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(VERTEX_POSCOLOR);
		UINT offset = 0;
		ID3D11Buffer *const vertexBufferPtr = VertexBufferManager::GetReference().GetPositionColorBuffer().GetVertexBuffer();
		Renderer::theContextPtr->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);

		ContextSharedRenderFunc(node);		
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Default);

	}
	//eVERTEX_POSBONEWEIGHTNORMTEX, eVERTEX_POSBONEWEIGHTNORMTANTEX,
	void RenderContext::ContextPosBoneWeightNormalUVRenderFuncSolution(RenderNode &node)
	{
		//static bool added = false;
		//if(!added)
		//{
		//	PrintConsole::GetReference().AddSolution(L"ED2", 1, Float3(1.0f, 0.0f, 0.0f), 
		//		L"RenderContext::ContextPosNormalTangentUVRenderFunc");
		//	added = true;
		//}
		
		RenderContext &context = (RenderContext &)node;
		// Set the Vertex Layout
		Renderer::theContextPtr->IASetInputLayout(
			InputLayoutManager::GetReference().GetInputLayout(eVERTEX_POSBONEWEIGHTNORMTEX));
		// Set the primitive type... Maybe this would be better in RenderShape so it can be specified by the mesh
		Renderer::theContextPtr->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(VERTEX_POSBONEWEIGHTNORMTEX);
		UINT offset = 0;
		ID3D11Buffer *const vertexBufferPtr = 
			VertexBufferManager::GetReference().GetPosBoneWeightNormTexBuffer().GetVertexBuffer();
		Renderer::theContextPtr->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);

		ContextSharedRenderFunc(node);
	}

	void RenderContext::ContextPosBoneWeightNormalTanUVRenderFuncSolution(RenderNode &node)
	{
		//static bool added = false;
		//if(!added)
		//{
		//	PrintConsole::GetReference().AddSolution(L"ED2", 1, Float3(1.0f, 0.0f, 0.0f), 
		//		L"RenderContext::ContextPosNormalTangentUVRenderFunc");
		//	added = true;
		//}
		
		RenderContext &context = (RenderContext &)node;
		// Set the Vertex Layout
		Renderer::theContextPtr->IASetInputLayout(
			InputLayoutManager::GetReference().GetInputLayout(eVERTEX_POSBONEWEIGHTNORMTANTEX));
		// Set the primitive type... Maybe this would be better in RenderShape so it can be specified by the mesh
		Renderer::theContextPtr->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(VERTEX_POSBONEWEIGHTNORMTANTEX);
		UINT offset = 0;
		ID3D11Buffer *const vertexBufferPtr = 
			VertexBufferManager::GetReference().GetPosBoneWeightNormTanTexBuffer().GetVertexBuffer();
		Renderer::theContextPtr->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);

		ContextSharedRenderFunc(node);
	}
}