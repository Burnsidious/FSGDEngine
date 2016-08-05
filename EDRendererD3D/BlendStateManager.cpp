#include "precompiled.h"
#include "BlendStateManager.h"
#include "Renderer.h"

namespace EDRendererD3D
{
	BlendStateManager *BlendStateManager::instancePtr = 0;

	BlendStateManager::BlendStateManager(void)
	{
		currentState = BS_COUNT;
	}


	BlendStateManager::~BlendStateManager(void)
	{
	}

	BlendStateManager &BlendStateManager::GetReference()
	{
		if(!instancePtr)
		{
			instancePtr = new BlendStateManager;
			instancePtr->CreateStates();
		}
		return *instancePtr;
	}

	BlendStateManager *BlendStateManager::GetInstance()
	{
		if(!instancePtr)
		{
			instancePtr = new BlendStateManager;
			instancePtr->CreateStates();
		}
		return instancePtr;
	}

	void BlendStateManager::DeleteInstance()
	{
		delete instancePtr;
		instancePtr = nullptr;
	}

	bool BlendStateManager::ApplyState(BStates state)
	{
		if(state >= BS_COUNT)
			return false;

		Renderer::theContextPtr->OMSetBlendState(blendStates[state], 0, 0xffffffff);
		currentState = state;
		return true;
	}

	void BlendStateManager::CreateStates()
	{
		D3D11_BLEND_DESC desc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		desc.RenderTarget[0].BlendEnable = FALSE;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		Renderer::theDevicePtr->CreateBlendState(&desc, &blendStates[BS_Default].p);

		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		Renderer::theDevicePtr->CreateBlendState(&desc, &blendStates[BS_Alpha].p);

		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		Renderer::theDevicePtr->CreateBlendState(&desc, &blendStates[BS_Additive].p);
	}
}