#include "precompiled.h"

#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/RenderSet.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDUtilities/InternalOutput.h"
using EDUtilities::InternalOutput;
using EDUtilities::PrintConsole;
#include "../EDMath/Float3.h"
using EDMath::Float3;

namespace EDRendererD3D
{
	void Renderer::InitializeSolution(HWND hWnd, bool fullscreen)
	{
		static bool added = false;
		if (!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f),
				L"Renderer::InitializeSolution");
			added = true;
		}

		// Describe the swap chain
		DXGI_SWAP_CHAIN_DESC swapChainDesc;

		int count = 1;
		int quality = 0;

		/*EDUtilities::Settings::GetInstance()->GetSetting("MSAACount",
			count, 1);

		EDUtilities::Settings::GetInstance()->GetSetting("MSAAQuaility",
			quality, 0);*/

		//D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT
		//D3D11_STANDARD_MULTISAMPLE_PATTERN
		//D3D11_CENTER_MULTISAMPLE_PATTERN
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = resolutionWidth;
		swapChainDesc.BufferDesc.Height = resolutionHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.SampleDesc.Count = count;
		swapChainDesc.SampleDesc.Quality = quality;
		swapChainDesc.Windowed = true;

		UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
		flags = flags | D3D11_CREATE_DEVICE_DEBUG;
#endif

		// With the swap chain described, create the device and the swap chain
		D3D_FEATURE_LEVEL supported;
		D3D_FEATURE_LEVEL requested[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3 };

		IDXGIFactory *factoryPtr = nullptr;
		IDXGIAdapter *adapterPtr = nullptr;
		std::vector<IDXGIAdapter *> adapters;

		CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factoryPtr);

		uint bestAdapterIndex = 0;
		size_t bestMemSize = 0;

		for (UINT i = 0;
			factoryPtr->EnumAdapters(i, &adapterPtr) != DXGI_ERROR_NOT_FOUND;
			++i)
		{
			adapters.push_back(adapterPtr);
			DXGI_ADAPTER_DESC desc;
			adapterPtr->GetDesc(&desc);

			if (desc.DedicatedVideoMemory > bestMemSize)
			{
				bestAdapterIndex = i;
				bestMemSize = desc.DedicatedVideoMemory;
			}
		}


		factoryPtr->Release();
		//bestAdapterIndex = 1;
		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			adapters[bestAdapterIndex], D3D_DRIVER_TYPE_UNKNOWN,
			//NULL, D3D_DRIVER_TYPE_HARDWARE,
			NULL, flags,
			requested, ARRAYSIZE(requested), D3D11_SDK_VERSION, &swapChainDesc,
			&theSwapChainPtr, &theDevicePtr, &supported, &theContextPtr);

		if (FAILED(hr))
		{
			InternalOutput::GetReference().Error("Failed to create the device\n");
			return;
		}
		if (supported != D3D_FEATURE_LEVEL_11_1 && supported != D3D_FEATURE_LEVEL_11_0)
		{
			InternalOutput::GetReference().Error("DirectX 11 support not available\n");
			if (supported == D3D_FEATURE_LEVEL_10_1)
				InternalOutput::GetReference().Error("Using feature level 10.1, some systems may fail\n");
			else if (supported == D3D_FEATURE_LEVEL_10_0)
				InternalOutput::GetReference().Error("Using feature level 10.0, some systems may fail\n");
			else
				InternalOutput::GetReference().Error("DirectX 10 support not available, time to get a new machine\n");
		}

		SetD3DName(theDevicePtr, "The Device");
		SetD3DName(theContextPtr, "The Context");
		SetD3DName(theSwapChainPtr, "The Swap Chain");

		if (fullscreen)
		{
			// Tell the fullscreen to be a specfic resolution
			DXGI_MODE_DESC md;
			// paramters of zero tell the swapChain to try and reuse existing settings.
			ZeroMemory(&md, sizeof(DXGI_MODE_DESC));
			md.Format = DXGI_FORMAT_UNKNOWN;
			md.Width = resolutionWidth;
			md.Height = resolutionHeight;
			theSwapChainPtr->ResizeTarget(&md);
			// forces fullscreen mode if requested
			hr = theSwapChainPtr->SetFullscreenState(true, 0);
		}

		if (hr != S_OK)
			InternalOutput::GetReference().Error("Setting Fullscreen has returned an error\n");
		BuildPerObjectConstantBuffers();
	}

	void Renderer::ResizeBuffersSolution()
	{
		ReleaseCOM(theRenderTargetViewPtr);
		ReleaseCOM(theDepthStencilBufferPtr);
		ReleaseCOM(theDepthStencilViewPtr);
		ReleaseCOM(theBackBufferPtr);

		HRESULT hr;

		theSwapChainPtr->ResizeBuffers(0, resolutionWidth, resolutionHeight,
			DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		HR(theSwapChainPtr->GetBuffer(0, __uuidof(theBackBufferPtr),
			reinterpret_cast<void**>(&theBackBufferPtr)));
		SetD3DName(theBackBufferPtr, "theBackBufferPtr");
		// Create a render-target view using our device
		theDevicePtr->CreateRenderTargetView(theBackBufferPtr, NULL, &theRenderTargetViewPtr);

		DXGI_SWAP_CHAIN_DESC swapDesc;
		theSwapChainPtr->GetDesc(&swapDesc);
		//width = swapDesc.BufferDesc.Width;
		//height = swapDesc.BufferDesc.Height;

		// Create texture description for depth buffer
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = swapDesc.BufferDesc.Width;
		depthStencilDesc.Height = swapDesc.BufferDesc.Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthStencilDesc.SampleDesc = swapDesc.SampleDesc;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		// Create depth buffer and view
		hr = theDevicePtr->CreateTexture2D(&depthStencilDesc, 0, &theDepthStencilBufferPtr);
		HR(hr);
		SetD3DName(theDepthStencilBufferPtr, "The Depth-Stencil Buffer Object");
		D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
		dsViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		//dsViewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;// | D3D11_DSV_READ_ONLY_STENCIL;
		dsViewDesc.Flags = 0;
		dsViewDesc.Texture2D.MipSlice = 0;
		hr = theDevicePtr->CreateDepthStencilView(theDepthStencilBufferPtr, &dsViewDesc,
			&theDepthStencilViewPtr);
		HR(hr);
		SetD3DName(theDepthStencilViewPtr, "The Depth Buffer View");

		// Bind the view to the immediate context (use .p when you want to interpret a com ptr as an array)
		theContextPtr->OMSetRenderTargets(1, &theRenderTargetViewPtr, theDepthStencilViewPtr);

		// Setup the viewport covering the entire rendertarget
		theScreenViewport.Width = float(swapDesc.BufferDesc.Width);
		theScreenViewport.Height = float(swapDesc.BufferDesc.Height);
		theScreenViewport.MinDepth = 0.0f;
		theScreenViewport.MaxDepth = 1.0f;
		theScreenViewport.TopLeftX = 0;
		theScreenViewport.TopLeftY = 0;
		theContextPtr->RSSetViewports(1, &theScreenViewport);
	}
	
	void Renderer::RenderSolution(RenderSet &set)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"Renderer::RenderSolution");
			added = true;
		}
		RenderNode *pCurrent = set.GetHead();

		while (0 != pCurrent)
		{
			pCurrent->RenderProcess();
			pCurrent = pCurrent->GetNext();
		}
	}
}