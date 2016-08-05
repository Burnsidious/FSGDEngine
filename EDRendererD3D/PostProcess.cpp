#include "precompiled.h"
#include "PostProcess.h"
#include "ShaderEffect.h"

#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/IndexBuffer.h"

#include "../EDUtilities/InternalOutput.h"
using EDUtilities::InternalOutput;

namespace EDRendererD3D
{
	PostProcess::~PostProcess()
	{
	}

	void PostProcess::Initialize(RenderMesh *_meshPtr, const char *vertexShaderName, 
			const char *pixelShaderName)
	{
		pixelShader.Release();
		vertexShader.Release();

		meshPtr = _meshPtr;

		// Load the vertex shader
		char *byteCode;
		size_t byteCodeSize;
		if(!ShaderEffect::LoadCompiledShaderData(&byteCode, byteCodeSize, vertexShaderName))
		{
			InternalOutput::GetReference().Error(
				"Failed to open vertex shader file %s", vertexShaderName);
			return;
		}

		HRESULT hr = Renderer::theDevicePtr->CreateVertexShader(
			byteCode, byteCodeSize, nullptr, &vertexShader);
		
		delete [] byteCode;

		if(S_OK != hr)
		{
			HR(hr);
			InternalOutput::GetReference().Error(
				"Failed to compile vertex shader file %s", vertexShaderName);
			return;
		}

		// Load the pixel shader
		if(!ShaderEffect::LoadCompiledShaderData(&byteCode, byteCodeSize, pixelShaderName))
		{
			InternalOutput::GetReference().Error(
				"Failed to open pixel shader file %s", pixelShaderName);
			return;
		}

		hr = Renderer::theDevicePtr->CreatePixelShader(
			byteCode, byteCodeSize, nullptr, &pixelShader);
		
		delete [] byteCode;

		if(S_OK != hr)
		{
			HR(hr);
			InternalOutput::GetReference().Error(
				"Failed to compile pixel shader file %s", pixelShaderName);
			return;
		}
	}

	void PostProcess::Process(RenderTarget &destinationTarget, ID3D11ShaderResourceView *sourceTexture)
	{
		// Implement your own solution for the Post-process lab
		return ProcessSolution(destinationTarget, sourceTexture);
	}
}