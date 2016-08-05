#pragma once

#include "RenderTarget.h"

namespace EDRendererD3D
{
	class CubeRenderTargetView : public RenderTargetSurface
	{
	public:
		CubeRenderTargetView();
		~CubeRenderTargetView();
		void Create(UINT size, DXGI_FORMAT format, const char * name);

		inline ID3D11RenderTargetView *GetRenderTargetView(UINT index) 
		{ return GetTexture()->GetRenderTargetView(index); }

		inline ID3D11RenderTargetView *GetRenderTargetView() 
		{ return GetTexture()->GetRenderTargetView(); }
	private:
	};

    /// An object for defining a texture target for rendering.
	class CubeRenderTarget : public RenderTarget
    {
    private:
        
    public:
        CubeRenderTarget();
        ~CubeRenderTarget();

        /// Initializes this render target.
        /// \param size - The size of the render target.
        /// \param format - The pixel format for this render target.
        void Create(UINT _size, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);
        void Create(UINT _size, UINT ignored, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT)
		{
			Create(_size, format);
		}

        /// Sets this as the current render target.
        /// \param _targetNo - The index of the surface this render target 
        /// should replace.  0 = COLOR0, 1 = COLOR1, etc.
        /// \param _surfaceNo - The index of the surface of the cube map to 
        /// use
        void ActivateTarget(DWORD _surfaceNo);
        void ActivateTarget();

		bool AddTarget(CubeRenderTargetView *view);

		//void Destroy();

		void ClearRenderTargetView(const FLOAT colorRGBA[4], DWORD _surfaceNo);

		inline ID3D11ShaderResourceView *GetShaderResourceView(UINT i = 0) 
		{ 
			return renderTargets[i]->GetShaderResourceView(); 
		}
    };
}