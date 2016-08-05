#include "precompiled.h"
#include "RenderTarget.h"
#include "Renderer.h"

#include "../EDMemoryManager/MemoryManager.h"
using namespace EDMemoryManager;



namespace EDRendererD3D
{
	RenderTargetSurface::RenderTargetSurface()
	{
		textureTargetPtr = 0;
	}

	RenderTargetSurface::~RenderTargetSurface()
	{
		Destroy();
	}

	void RenderTargetSurface::Destroy()
	{
		if(textureTargetPtr)
		{
			delete textureTargetPtr;
			textureTargetPtr = 0;
		}
	}

	void RenderTargetSurface::Create(UINT _width, UINT _height, DXGI_FORMAT format, const char * name)
	{
		width = _width;
		height = _height;
        // Implement a solution for the Deferred Renderer Lab
		return CreateSolution(_width, _height, format, name);
	}

    RenderTarget::RenderTarget()
    {
        depthStecilView = 0;
		depthTexturePtr = 0;

		bShouldDeleteDepth = true;
    }

    RenderTarget::~RenderTarget()
    {
		Destroy();
    }

    void RenderTarget::Create(UINT _width, UINT _height, DXGI_FORMAT format)
    {
		Destroy();
		bShouldDeleteDepth = true;

        // Implement a solution for the Deferred Renderer Lab
        return CreateSolution(_width, _height, format);
    }

	void RenderTarget::Create(ID3D11DepthStencilView * _depthStecilView,
		ID3D11Texture2D *_depthTexturePtr)
	{
		Destroy();
		bShouldDeleteDepth = false;

		depthTexturePtr = _depthTexturePtr;
		depthStecilView = _depthStecilView;
	}

    void RenderTarget::ActivateTarget(DWORD ignored)
    {
        // Implement a solution for the Deferred Renderer Lab
        return ActivateTargetSolution();
    }

	bool RenderTarget::AddTarget(RenderTargetSurface *view)
	{
		renderTargets.push_back(view);
		renderTargetsSRVs.push_back(view->GetShaderResourceView());
		return true;
	}

	void RenderTarget::Destroy()
	{
		renderTargets.clear();
		renderTargetsSRVs.clear();
	}

	void RenderTarget::ClearRenderTargetView(const FLOAT colorRGBA[4])
	{
		size_t size = renderTargets.size();
		
		for(size_t i = 0; i < size; ++i)
		{
			Renderer::theContextPtr->ClearRenderTargetView(renderTargets[i]->GetRenderTargetView(),
				colorRGBA);
		}
	}

	void RenderTarget::ClearDepthStencilView(UINT clearFlags, FLOAT depth, UINT8 stencil)
	{
		if(depthStecilView)
			Renderer::theContextPtr->ClearDepthStencilView(depthStecilView, clearFlags, depth,
				stencil);
	}

	void RenderTarget::ActivateSRVs(UINT startSlot)
	{
		Renderer::theContextPtr->PSSetShaderResources(startSlot, 4, &renderTargetsSRVs[0]);
		Renderer::theContextPtr->DSSetShaderResources(startSlot, 4, &renderTargetsSRVs[0]);
	}

	void RenderTarget::DeactivateSRVs(UINT startSlot)
	{
		ID3D11ShaderResourceView *nullSRVs[128] = { 0, };
		Renderer::theContextPtr->PSSetShaderResources(startSlot, 4, nullSRVs);
		Renderer::theContextPtr->DSSetShaderResources(startSlot, 4, nullSRVs);

	}
}