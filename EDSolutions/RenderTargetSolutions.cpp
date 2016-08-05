
#include "precompiled.h"

#include "../EDRendererD3D/RenderTarget.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDUtilities/PrintConsole.h"
using EDUtilities::PrintConsole;
#include "../EDMath/Float3.h"
using EDMath::Float3;

#include "../EDMemoryManager/MemoryManager.h"
using namespace EDMemoryManager;


namespace EDRendererD3D
{
	void RenderTargetSurface::CreateSolution(UINT width, UINT height, DXGI_FORMAT format, const char * name)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Deferred Shading", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderTargetSurface::CreateSolution");
			added = true;
		}

		// Out with he old
		Destroy();

		// Describe the new texture to be created
		D3D11_TEXTURE2D_DESC texDesc;

		texDesc.Width     = width;
		texDesc.Height    = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format    = format;
		texDesc.SampleDesc.Count   = 1;  
		texDesc.SampleDesc.Quality = 0;  
		texDesc.Usage          = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags      = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0; 
		texDesc.MiscFlags      = 0;

		// Create an engine resource texture to hold the created D3D texture
		textureTargetPtr = new RenderTexture2D();
		textureTargetPtr->Create(texDesc, name);
	}

	void RenderTarget::CreateSolution(UINT _width, UINT _height, DXGI_FORMAT format)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Deferred Shading", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderTarget::CreateSolution");
			added = true;
		}

		// Create texture description for depth buffer
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width     = _width;
		depthStencilDesc.Height    = _height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format    = format;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0; 
		depthStencilDesc.MiscFlags      = 0;

		// Create depth buffer and view
		HR(Renderer::theDevicePtr->CreateTexture2D(&depthStencilDesc, 0, &depthTexturePtr));
		SetD3DName(depthTexturePtr, "RenderTarget depth texture");
		HR(Renderer::theDevicePtr->CreateDepthStencilView(depthTexturePtr, 0, 
			&depthStecilView));
		SetD3DName(depthStecilView, "RenderTarget Depth Buffer View");
	}

	void RenderTarget::ActivateTargetSolution()
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Deferred Shading", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderTarget::ActivateTargetSolution");
			added = true;
		}
		size_t size = renderTargets.size();

		// We should not be activating if there are no targets
		assert(size);
		ID3D11RenderTargetView *views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
		for(size_t i = 0; i < size; ++i)
			views[i] = renderTargets[i]->GetRenderTargetView();
		Renderer::theContextPtr->OMSetRenderTargets(static_cast<UINT>(size), views, depthStecilView);
		//Renderer::theContextPtr->OMSetRenderTargets(static_cast<UINT>(size), views, Renderer::theDepthStencilViewPtr);
	}
}