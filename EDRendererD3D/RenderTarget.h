#pragma once

#include "RenderTexture2D.h"

namespace EDRendererD3D
{
	class RenderTargetSurface
	{
	public:
		RenderTargetSurface();
		virtual ~RenderTargetSurface();
		virtual void Destroy();
		void Create(UINT width, UINT height, DXGI_FORMAT format, const char * name);

        inline RenderTexture *GetTexture() { return textureTargetPtr; }
		inline ID3D11RenderTargetView *GetRenderTargetView() { return GetTexture()->GetRenderTargetView(); }
		inline ID3D11ShaderResourceView *GetShaderResourceView() { return GetTexture()->GetShaderResourceView(); }

		UINT GetWidth() { return width; }
		UINT GetHeight() { return height; }

		RenderTargetSurface &operator=(const RenderTargetSurface &rhs)
		{
			delete textureTargetPtr;
			textureTargetPtr = new RenderTexture2D;

			*textureTargetPtr = *rhs.textureTargetPtr;

			return *this;
		}

	protected:
		UINT width;
		UINT height;
		RenderTexture * textureTargetPtr;
		void CreateSolution(UINT width, UINT height, DXGI_FORMAT format, const char * name);

	};
    /// An object for defining a texture target for rendering.
	class RenderTarget 
    {
    protected:
		CComPtr<ID3D11DepthStencilView> depthStecilView;
		CComPtr<ID3D11Texture2D> depthTexturePtr;

		std::vector<RenderTargetSurface *> renderTargets;
		std::vector<ID3D11ShaderResourceView *> renderTargetsSRVs;
		
		bool bShouldDeleteDepth;

    public:
        RenderTarget();
        virtual ~RenderTarget();

        /// Initializes this render target.
        /// \param width - The width of the render target.
        /// \param length - The length of the render target.
        /// \param format - The pixel format for this render target.
        virtual void Create(UINT _width, UINT _height, 
            DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);
		void Create(ID3D11DepthStencilView * _depthStecilView, 
			ID3D11Texture2D *_depthTexturePtr);
        /// Sets this as the current render target.
		virtual void ActivateTarget(DWORD ignored = -1);
		void ActivateSRVs(UINT startSlot);
		void DeactivateSRVs(UINT startSlot);

		bool AddTarget(RenderTargetSurface *view);

		inline RenderTargetSurface *GetRenderTargetSurface(unsigned int index)
		{
			return renderTargets[index];
		}

		inline ID3D11ShaderResourceView *GetShaderResourceView(UINT i = 0) 
		{ 
			return renderTargets[i]->GetShaderResourceView(); 
		}

		void Destroy();

		void ClearRenderTargetView(const FLOAT colorRGBA[4]);
		void ClearDepthStencilView(UINT clearFlags, FLOAT depth = 1.0F, UINT8 stencil = 0);

		size_t GetNumBuffers(void) const { return renderTargets.size(); }

        /// Solution declarations, ignore these
        void CreateSolution(UINT _width, UINT _height, DXGI_FORMAT format);
        /// Solution declarations, ignore these
        void ActivateTargetSolution();
        /// Solution declarations, ignore these
        void RevertTargetSolution();
    };
}