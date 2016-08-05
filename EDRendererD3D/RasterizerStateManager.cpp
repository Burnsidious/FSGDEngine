#include "precompiled.h"
#include "RasterizerStateManager.h"
#include "Renderer.h"

namespace EDRendererD3D
{
    RasterizerStateManager *RasterizerStateManager::instancePtr = 0;

	RasterizerStateManager::RasterizerStateManager(void)
	{
		currentState = RS_COUNT;
	}


	RasterizerStateManager::~RasterizerStateManager(void)
	{
	}

    RasterizerStateManager &RasterizerStateManager::GetReference()
    {
        if(!instancePtr)
        {
			instancePtr = new RasterizerStateManager;
			instancePtr->CreateStates();
        }
        return *instancePtr;
    }

    void RasterizerStateManager::DeleteInstance()
    {
		delete instancePtr;
		instancePtr = nullptr;
    }

	bool RasterizerStateManager::ApplyState(RasterStates state)
	{
		if(state >= RS_COUNT)
			return false;

		Renderer::theContextPtr->RSSetState(rasterStates[state]);
		currentState = state;
		return true;
	}

	void RasterizerStateManager::CreateStates()
	{
		D3D11_RASTERIZER_DESC desc;

		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = 0;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthBiasClamp = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;
		Renderer::theDevicePtr->CreateRasterizerState(&desc, &rasterStates[RS_Default].p);
		
		desc.FillMode = D3D11_FILL_WIREFRAME;
		Renderer::theDevicePtr->CreateRasterizerState(&desc, &rasterStates[RS_LINE].p);
		desc.FillMode = D3D11_FILL_SOLID;

		desc.FrontCounterClockwise = TRUE;
		Renderer::theDevicePtr->CreateRasterizerState(&desc, &rasterStates[RS_CCW].p);

		desc.CullMode = D3D11_CULL_NONE;
		Renderer::theDevicePtr->CreateRasterizerState(&desc, &rasterStates[RS_NOCULL].p);

	}
}