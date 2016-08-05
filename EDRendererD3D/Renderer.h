#pragma once
#include "ConstantBuffers.h"
#include "RenderNode.h"

namespace EDRendererD3D
{
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)               \
	{                           \
		HRESULT macroHR = (x);  \
		if(FAILED(macroHR))     \
		{                       \
		assert(false);			\
		}                       \
	}
	#endif
#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef SetD3DName
	#define SetD3DName(object, name)							\
	{															\
		if(name)												\
			object->SetPrivateData(WKPDID_D3DDebugObjectName,	\
				(UINT)strlen( name ), name );					\
	}
	#endif
#else
	#ifndef SetD3DName
	#define SetD3DName(object, name) 
	#endif
#endif 

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

    class RenderSet;
	/// Renderer used to render RenderNodes contained in RenderSet instances.
    class Renderer
    {
    public:
		Renderer(void) {}
		~Renderer(void) {}

		/// Interface for feature support queries and resource allocation
		static ID3D11Device * theDevicePtr;
		/// Interface to rendering commands, close relation to D3D9 device interface
		static ID3D11DeviceContext * theContextPtr;
		/// Interface for storing rendered data and presenting
		static IDXGISwapChain * theSwapChainPtr;

		/// Interface view to the back buffer
		static ID3D11RenderTargetView * theRenderTargetViewPtr;
		/// Interface to the texture representing our back buffer
		static ID3D11Texture2D *theBackBufferPtr;

		/// Interface to the texture representing the depth buffer
		static ID3D11Texture2D * theDepthStencilBufferPtr;
		/// Interface view to the depth buffer
		static ID3D11DepthStencilView  * theDepthStencilViewPtr;

		/// Defines the demensions of the viewport we render to
		static D3D11_VIEWPORT theScreenViewport;

		static cbPerObject thePerObjectData;
		static ID3D11Buffer *thePerObjectCBuffer;
		static void SetPerObjectData(DirectX::XMFLOAT4X4 &mMVP, DirectX::XMFLOAT4X4 &mWorld);
		static void SetPerObjectData(DirectX::XMMATRIX &mMVP, DirectX::XMMATRIX &mWorld);

		static cbPerSkinnedObject thePerSkinnedObjectData;
		static ID3D11Buffer *thePerSkinnedObjectCBuffer;
		static void SetPerObjectData(DirectX::XMFLOAT4X4 &mViewProj, DirectX::XMFLOAT4X4 &mWorld, 
			DirectX::XMFLOAT4X4 SkinnedMatrices[cbPerSkinnedObject::NUM_BONES]);

		/// Initializes the D3D rendering objects
		/// \param hWnd - The handle to the window to attach D3D to.
		static void Initialize(HWND hWnd, UINT resWidth, UINT resHeight);

		static void SetResolution(UINT _width, UINT _height);

		/// Releases the D3D/COM objects
        static void Shutdown();

        /// Renders a batch of RenderNode instances contained in the RenderSet passed in.
		/// \param set - The batch to be rendered.
        static void Render(RenderSet &set);

        static void Render(RenderSet &set, EDRenderFunc renderFuncOverride);


		inline static UINT GetRenderNumber(void){ return theRenderCounter; }
		inline static void IncrementRenderCounter(void){ ++theRenderCounter; }

		/// Clears the Render Target to the specified color
		/// \param clearColor - an array of four floats representing the desired clear color
		inline static void ClearRenderTarget(const FLOAT clearColor[4])
		{
			theContextPtr->ClearRenderTargetView(theRenderTargetViewPtr, clearColor);
		}

		/// Clears the depth and stencil buffer
		/// \param clearFlags - Specifies portions of the buffer to be cleared, approriate 
		/// values for this flag can be ORed and include D3D10_CLEAR_DEPTH and D3D10_CLEAR_STENCIL
		/// \param depth - If clearing depth, the value to clear to
		/// \param stencil - If clearing the stencil, the value to clear to
		inline static void ClearDepthAndStencilTarget(
			UINT clearFlags = D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, FLOAT depth = 1.0f,
			UINT8 stencil = 0)
		{
			theContextPtr->ClearDepthStencilView(theDepthStencilViewPtr, clearFlags, depth, stencil);
		}

		/// Presents the next buffer. 
		inline static void Present( UINT syncInterval = 0, UINT flags = 0)
		{
			theSwapChainPtr->Present(syncInterval, flags);
		}

		inline static UINT GetResolutionWidth() { return resolutionWidth; }
		inline static UINT GetResolutionHeight() { return resolutionHeight; }

		static ID3D11ShaderResourceView *GetDepthSRV();

		/// Creates the render target view, depth and stencil buffer and view.  This method 
		/// should be called each time the window changes size
		static void ResizeBuffers();

	private:
		static UINT theRenderCounter;
		static UINT resolutionWidth;
		static UINT resolutionHeight;

		static ID3D11ShaderResourceView *theDepthStencilSRVPtr;

		static void BuildPerObjectConstantBuffers();

		static void InitializeSolution(HWND hWnd, bool fullscreen);
		static void RenderSolution(RenderSet &set);
		static void ResizeBuffersSolution();	
	};
}