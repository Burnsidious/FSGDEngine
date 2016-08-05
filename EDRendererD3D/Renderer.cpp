#include "precompiled.h"
#include "Renderer.h"
#include "RenderNode.h"
#include "RenderSet.h"

#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/Settings.h"

using EDUtilities::InternalOutput;


using std::ifstream;

using namespace DirectX;

namespace EDRendererD3D
{
    ID3D11Device * Renderer::theDevicePtr = 0;
    ID3D11DeviceContext * Renderer::theContextPtr = 0;
    IDXGISwapChain * Renderer::theSwapChainPtr = 0;
    ID3D11RenderTargetView * Renderer::theRenderTargetViewPtr = 0;
	ID3D11Texture2D * Renderer::theBackBufferPtr = 0;
    ID3D11Texture2D * Renderer::theDepthStencilBufferPtr = 0;
    ID3D11DepthStencilView * Renderer::theDepthStencilViewPtr = 0;
	ID3D11ShaderResourceView *Renderer::theDepthStencilSRVPtr = 0;

	D3D11_VIEWPORT Renderer::theScreenViewport;
	cbPerObject Renderer::thePerObjectData;
	cbPerSkinnedObject Renderer::thePerSkinnedObjectData;
	ID3D11Buffer *Renderer::thePerObjectCBuffer = 0;
	ID3D11Buffer *Renderer::thePerSkinnedObjectCBuffer = 0;
	UINT Renderer::resolutionWidth = 0;
	UINT Renderer::resolutionHeight = 0;

	UINT Renderer::theRenderCounter = 1;

	bool fullscreen = true;

	void Renderer::Initialize(HWND hWnd, UINT resWidth, UINT resHeight)
    {
		bool isFullscreen;
		EDUtilities::Settings::GetInstance()->GetSetting("Fullscreen", isFullscreen, false);

		resolutionWidth = resWidth;
		resolutionHeight = resHeight;

		return InitializeSolution(hWnd, isFullscreen);
	}

	void Renderer::ResizeBuffers()
	{
		return ResizeBuffersSolution();
	}

	void Renderer::SetResolution(UINT _width, UINT _height)
	{
		// We have to release all references to the swap chain before we can resize it
		ReleaseCOM(theRenderTargetViewPtr);
		ReleaseCOM(theDepthStencilBufferPtr);
		ReleaseCOM(theDepthStencilViewPtr);
		ReleaseCOM(theBackBufferPtr);

		resolutionWidth = _width;
		resolutionHeight = _height;
		ResizeBuffers();		
	}

    void Renderer::Shutdown()
    {	
		theSwapChainPtr->SetFullscreenState(false,0);

        // release the d3d object and device
		ReleaseCOM(theDepthStencilViewPtr);
		ReleaseCOM(theDepthStencilBufferPtr);
		ReleaseCOM(theRenderTargetViewPtr);
		ReleaseCOM(theBackBufferPtr);
		ReleaseCOM(thePerObjectCBuffer);
		ReleaseCOM(thePerSkinnedObjectCBuffer);
		ReleaseCOM(theDepthStencilSRVPtr);

		ReleaseCOM(theSwapChainPtr);
		ReleaseCOM(theContextPtr);
		ReleaseCOM(theDevicePtr);
    }

    void Renderer::Render(RenderSet &set)
    {
        // Implement a solution for the Renderer Lab
        return RenderSolution(set);
    }

	void Renderer::SetPerObjectData(XMFLOAT4X4 &mMVP, XMFLOAT4X4 &mWorld)
	{
		thePerObjectData.gMVP = mMVP;
		thePerObjectData.gWorld = mWorld;

		D3D11_MAPPED_SUBRESOURCE edit;
		theContextPtr->Map(thePerObjectCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, &thePerObjectData, sizeof(thePerObjectData));
		theContextPtr->Unmap(thePerObjectCBuffer, 0);

		theContextPtr->VSSetConstantBuffers(cbPerObject::REGISTER_SLOT, 1, &thePerObjectCBuffer);
		theContextPtr->PSSetConstantBuffers(cbPerObject::REGISTER_SLOT, 1, &thePerObjectCBuffer);
		theContextPtr->GSSetConstantBuffers(cbPerObject::REGISTER_SLOT, 1, &thePerObjectCBuffer);
		theContextPtr->DSSetConstantBuffers(cbPerObject::REGISTER_SLOT, 1, &thePerObjectCBuffer);
		theContextPtr->HSSetConstantBuffers(cbPerObject::REGISTER_SLOT, 1, &thePerObjectCBuffer);
	}

	void Renderer::SetPerObjectData(XMMATRIX &mMVP, XMMATRIX &mWorld)
	{
		XMFLOAT4X4 tempMVP;
		XMFLOAT4X4 tempWorld;

		XMStoreFloat4x4(&tempMVP, mMVP);
		XMStoreFloat4x4(&tempWorld, mWorld);

		SetPerObjectData(tempMVP, tempWorld);
	}

	void Renderer::SetPerObjectData(DirectX::XMFLOAT4X4 &mViewProj, 
		XMFLOAT4X4 &mWorld,	XMFLOAT4X4 SkinnedMatrices[cbPerSkinnedObject::NUM_BONES])
	{
		thePerSkinnedObjectData.gSkinnedViewProj = mViewProj;
		//thePerSkinnedObjectData.gSkinnedWorld = mWorld;
		
		memcpy(thePerSkinnedObjectData.gSkinnedMatrices, SkinnedMatrices, 
			cbPerSkinnedObject::NUM_BONES * sizeof(XMFLOAT4X4));

		D3D11_MAPPED_SUBRESOURCE edit;
		theContextPtr->Map(thePerSkinnedObjectCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, &thePerSkinnedObjectData, sizeof(thePerSkinnedObjectData));
		theContextPtr->Unmap(thePerSkinnedObjectCBuffer, 0);

		theContextPtr->VSSetConstantBuffers(cbPerSkinnedObject::REGISTER_SLOT, 1,
			&thePerSkinnedObjectCBuffer);
		theContextPtr->PSSetConstantBuffers(cbPerSkinnedObject::REGISTER_SLOT, 1, 
			&thePerSkinnedObjectCBuffer);
		theContextPtr->GSSetConstantBuffers(cbPerSkinnedObject::REGISTER_SLOT, 1, 
			&thePerSkinnedObjectCBuffer);
		theContextPtr->DSSetConstantBuffers(cbPerSkinnedObject::REGISTER_SLOT, 1, 
			&thePerSkinnedObjectCBuffer);
		theContextPtr->HSSetConstantBuffers(cbPerSkinnedObject::REGISTER_SLOT, 1, 
			&thePerSkinnedObjectCBuffer);

	}

	ID3D11ShaderResourceView *Renderer::GetDepthSRV()
	{
		// Do not try to create a view to an uninitialized resource
		assert(theDepthStencilBufferPtr);
		if(!theDepthStencilSRVPtr)
		{
			// Create the requested view
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			//desc.Buffer = 0; //?
			desc.Texture2D.MipLevels = -1;
			desc.Texture2D.MostDetailedMip = 0;
			HRESULT hr = Renderer::theDevicePtr->CreateShaderResourceView(
				theDepthStencilBufferPtr, &desc, &theDepthStencilSRVPtr);
			HR(hr);
			SetD3DName(theDepthStencilSRVPtr, "Depth Stencil SRV");
		}
		return theDepthStencilSRVPtr; 
	}

	void Renderer::BuildPerObjectConstantBuffers()
	{
		// per object CBuffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(cbPerObject);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(theDevicePtr->CreateBuffer(&bd, nullptr, &thePerObjectCBuffer));

		// per object CBuffer
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(cbPerSkinnedObject);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(theDevicePtr->CreateBuffer(&bd, nullptr, &thePerSkinnedObjectCBuffer));	
	}

	void Renderer::Render(RenderSet &set, EDRenderFunc renderFuncOverride)
	{
		RenderNode *pCurrent = set.GetHead();

		while (0 != pCurrent)
		{
			renderFuncOverride(*pCurrent);
			pCurrent = pCurrent->GetNext();
		}
	}
}