#include "precompiled.h"
#include "CubeRenderTarget.h"
#include "Renderer.h"
#include "RenderTextureCubeMap.h"


#include "../EDMemoryManager/MemoryManager.h"
using namespace EDMemoryManager;

namespace EDRendererD3D
{
	CubeRenderTargetView::CubeRenderTargetView()
	{
		textureTargetPtr = 0;
	}

	CubeRenderTargetView::~CubeRenderTargetView()
	{
		Destroy();
	}

	void CubeRenderTargetView::Create(UINT size, DXGI_FORMAT format, const char * name)
	{
		// Out witht he old
		Destroy();

		// Describe the new texture to be created
		D3D11_TEXTURE2D_DESC texDesc;

		texDesc.Width     = size;
		texDesc.Height    = size;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 6;
		texDesc.Format    = format;
		texDesc.SampleDesc.Count   = 1;  
		texDesc.SampleDesc.Quality = 0;  
		texDesc.Usage          = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags      = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0; 
		texDesc.MiscFlags      = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// Create an engine resource texture to hold the created D3D texture
		textureTargetPtr = new RenderTextureCubeMap();
		textureTargetPtr->Create(texDesc, name);
	}
    CubeRenderTarget::CubeRenderTarget() : RenderTarget()
    {
    }

    CubeRenderTarget::~CubeRenderTarget()
    {
		Destroy();
    }

    void CubeRenderTarget::Create(UINT _size, DXGI_FORMAT format)
    {
		Destroy();

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width     = _size;
		depthStencilDesc.Height    = _size;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 6;
		//depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format    = format;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0; 
		depthStencilDesc.MiscFlags      = D3D11_RESOURCE_MISC_TEXTURECUBE;
		//depthStencilDesc.MiscFlags      = 0;

		// Create depth buffer and view
		HR(Renderer::theDevicePtr->CreateTexture2D(&depthStencilDesc, 0, &depthTexturePtr));
		SetD3DName(depthTexturePtr, "CubeRenderTarget depth texture");

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
		viewDesc.Format = format;
		viewDesc.Flags = 0;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.FirstArraySlice = 0;
		viewDesc.Texture2DArray.ArraySize = 6;
		viewDesc.Texture2DArray.MipSlice = 0;
		HR(Renderer::theDevicePtr->CreateDepthStencilView(depthTexturePtr, &viewDesc, 
			&depthStecilView));
		SetD3DName(depthStecilView, "CubeRenderTarget Depth Buffer View");
    }

	bool CubeRenderTarget::AddTarget(CubeRenderTargetView *view)
	{
		// Make sure we can have another target
		if(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT == renderTargets.size())
			return false;
		renderTargets.push_back(view);
		return true;
	}

    void CubeRenderTarget::ActivateTarget(DWORD _surfaceNo)
    {
		size_t count = renderTargets.size();

		// We should not be activating if there are no targets
		assert(count);
		ID3D11RenderTargetView *views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
		for(size_t i = 0; i < count; ++i)
			views[i] = static_cast<CubeRenderTargetView*>(renderTargets[i])->GetRenderTargetView(_surfaceNo);
		Renderer::theContextPtr->OMSetRenderTargets((UINT)count, views, depthStecilView);
    }

	 void CubeRenderTarget::ActivateTarget()
    {
		size_t count = renderTargets.size();

		// We should not be activating if there are no targets
		assert(count);
		ID3D11RenderTargetView *views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
		for(size_t i = 0; i < count; ++i)
			views[i] = static_cast<CubeRenderTargetView*>(renderTargets[i])->GetRenderTargetView();
		Renderer::theContextPtr->OMSetRenderTargets((UINT)count, views, depthStecilView);
    }

	void CubeRenderTarget::ClearRenderTargetView(const FLOAT colorRGBA[4], DWORD _surfaceNo)
	{
		size_t size = renderTargets.size();
		
		for(size_t i = 0; i < size; ++i)
		{
			Renderer::theContextPtr->ClearRenderTargetView(
				static_cast<CubeRenderTargetView*>(renderTargets[i])->GetRenderTargetView(_surfaceNo), 
				colorRGBA);
		}
	}
}