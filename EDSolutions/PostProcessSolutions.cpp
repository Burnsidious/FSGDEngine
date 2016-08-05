#include "precompiled.h"

#include "../EDRendererD3D/PostProcess.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/IndexBuffer.h"
#include "../EDUtilities/PrintConsole.h"
using EDUtilities::PrintConsole;
using EDMath::Float3;

#include "../EDUtilities/InternalOutput.h"
using EDUtilities::InternalOutput;

namespace EDRendererD3D
{
	void PostProcess::ProcessSolution(RenderTarget &destinationTarget, 
		ID3D11ShaderResourceView *sourceTexture)
	{
		destinationTarget.ActivateTarget();
		ID3D11Device *pDevice = Renderer::theDevicePtr;

		// Set the vertex and index source and declarion
		Renderer::theContextPtr->IASetInputLayout(
			InputLayoutManager::GetReference().GetInputLayout(eVERTEX_POSTEX));
		UINT stride = sizeof(VERTEX_POSTEX);
		UINT offset = 0;
		ID3D11Buffer *const vertexBufferPtr = 
			VertexBufferManager::GetReference().GetPositionTexBuffer().GetVertexBuffer();
		Renderer::theContextPtr->IASetVertexBuffers(0, 1, &vertexBufferPtr, &stride, &offset);
		Renderer::theContextPtr->IASetIndexBuffer(
			IndexBuffer::GetReference().GetIndices(), DXGI_FORMAT_R32_UINT, 0);

 		Renderer::theContextPtr->VSSetShader( vertexShader, NULL, 0 );
		Renderer::theContextPtr->PSSetShader( pixelShader, NULL, 0 );
		Renderer::theContextPtr->PSSetShaderResources(SOURCE_MAP_REGISTER, 1, &sourceTexture);

		Renderer::theContextPtr->DrawIndexed(meshPtr->GetPrimitiveCount() * 3, 
			meshPtr->GetStartIndex(), meshPtr->GetStartVertex());

		ID3D11ShaderResourceView *srv = 0;
		Renderer::theContextPtr->PSSetShaderResources(SOURCE_MAP_REGISTER, 1, &srv);
	}
}