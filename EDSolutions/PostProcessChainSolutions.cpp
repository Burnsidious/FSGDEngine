#include "precompiled.h"

#include "../FSGDGame/PostProcessChain.h"
#include "../FSGDGame/WorldData.h"
#include "../EDUtilities/PrintConsole.h"
#include "../FSGDGame/RenderController.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDMath/Float3.h"

using namespace EDRendererD3D;
using EDUtilities::PrintConsole;
using EDMath::Float3;
using std::list;

void PostProcessChain::InitializeSolution()
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Post-processing", Float3(1.0f, 0.0f, 0.0f), 
			L"PostProcessChain::InitializeSolution");
		added = true;
	}

	DXGI_FORMAT pixelFormat = DXGI_FORMAT_R10G10B10A2_UNORM;

	// Build initial render targets
	renderTarget1.Create(Renderer::theDepthStencilViewPtr, Renderer::theDepthStencilBufferPtr);
	renderTargetView1.Create(Renderer::GetResolutionWidth(), Renderer::GetResolutionHeight(), pixelFormat,
		"Post-process Target1");
	renderTarget1.AddTarget(&renderTargetView1);

	renderTarget2.Create(Renderer::theDepthStencilViewPtr, Renderer::theDepthStencilBufferPtr);
	renderTargetView2.Create(Renderer::GetResolutionWidth(), Renderer::GetResolutionHeight(), pixelFormat,
		"Post-process Target2");
	renderTarget2.AddTarget(&renderTargetView2);

	// initialize source and destination
	sourceTargetPtr = &renderTarget1;
	destinationTargetPtr = &renderTarget2;

	// Build buffer to interface with post process shader constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(cbPostProcess);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	postShaderDataCBuffer.Release();
	HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &postShaderDataCBuffer));

}

void PostProcessChain::RunSolution(RenderTarget &mainTarget)
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Post-processing", Float3(1.0f, 0.0f, 0.0f), 
			L"PostProcessChain::RunSolution");
		added = true;
	}

	list< PostProcessEffect >::iterator processIter = 
		processChain.begin();

	list< PostProcessEffect >::const_iterator processEnd = 
		processChain.end();

	if(processEnd == processIter)
		return;

	UpdateViewports(processIter);
	UpdateShaderData();

	// Run the first process using the passed in target as the source.
	(*processIter).postProcessPtr->Process(*destinationTargetPtr, 
		mainTarget.GetShaderResourceView());

	ID3D11ShaderResourceView *sharpTexture = mainTarget.GetShaderResourceView();
	Renderer::theContextPtr->PSSetShaderResources(PRE_POST_MAP_REGISTER, 1, &sharpTexture);

	++processIter;
	SwapTargets();

	while(processIter != processEnd)
	{
		UpdateViewports(processIter);
		UpdateShaderData();
		(*processIter).postProcessPtr->Process(*destinationTargetPtr, 
			sourceTargetPtr->GetShaderResourceView());

		SwapTargets();
		++processIter;
	}
	sharpTexture = 0;
	Renderer::theContextPtr->PSSetShaderResources(PRE_POST_MAP_REGISTER, 1, &sharpTexture);
}

void PostProcessChain::SwapTargetsSolution()
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Post-processing", Float3(1.0f, 0.0f, 0.0f), 
			L"PostProcessChain::SwapTargetsSolution");
		added = true;
	}

	RenderTarget *newDestination = sourceTargetPtr;

	sourceTargetPtr = destinationTargetPtr;

	destinationTargetPtr = newDestination;
}

void PostProcessChain::UpdateShaderDataSolution()
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Post-processing", Float3(1.0f, 0.0f, 0.0f), 
			L"PostProcessChain::UpdateShaderDataSolution");
		added = true;
	}

	D3D11_MAPPED_SUBRESOURCE edit;
	Renderer::theContextPtr->Map(postShaderDataCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &postShaderData, sizeof(postShaderData));
	Renderer::theContextPtr->Unmap(postShaderDataCBuffer, 0);

	Renderer::theContextPtr->VSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);
	Renderer::theContextPtr->PSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);
}

void PostProcessChain::UpdateViewportsSolution(std::list< PostProcessEffect >::iterator &processIter)
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Post-processing", Float3(1.0f, 0.0f, 0.0f), 
			L"PostProcessChain::UpdateViewportsSolution");
		added = true;
	}

	// Adjust scale of texture coordinates based on whether we are up or downsampling
	if(PPT_DownSample == sampleState )
		postShaderData.gTexCoordScale /= 2;
	else if(PPT_UpSample == sampleState)
		postShaderData.gTexCoordScale *= 2;

	if(processIter != processChain.end())
	{
		sampleState = (*processIter).transition;
		if(PPT_DownSample == sampleState)
		{
			D3D11_VIEWPORT smallPort;
			UINT num = 1;
			Renderer::theContextPtr->RSGetViewports(&num, &smallPort);
			smallPort.Width = smallPort.Width/2;
			smallPort.Height = smallPort.Height/2;
			Renderer::theContextPtr->RSSetViewports( 1, &smallPort );	
		}
		else if(PPT_UpSample == sampleState)
		{
			D3D11_VIEWPORT largePort;
			UINT num = 1;
			Renderer::theContextPtr->RSGetViewports(&num, &largePort);
			largePort.Width = largePort.Width*2;
			largePort.Height = largePort.Height*2;
			Renderer::theContextPtr->RSSetViewports( 1, &largePort );	
		}
	}


}