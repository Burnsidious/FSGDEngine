#include "precompiled.h"
#include "DepthStencilStateManager.h"
#include "Renderer.h"

namespace EDRendererD3D
{
    DepthStencilStateManager *DepthStencilStateManager::instancePtr = 0;

	DepthStencilStateManager::DepthStencilStateManager(void)
	{
		// purposely invalid option
		currentState = DSS_COUNT;
	}


	DepthStencilStateManager::~DepthStencilStateManager(void)
	{
	}

    DepthStencilStateManager &DepthStencilStateManager::GetReference()
    {
        if(!instancePtr)
        {
			instancePtr = new DepthStencilStateManager;
			instancePtr->CreateStates();
        }
        return *instancePtr;
    }

    void DepthStencilStateManager::DeleteInstance()
    {
		delete instancePtr;
		instancePtr = nullptr;
    }

	bool DepthStencilStateManager::ApplyState(DSStates state)
	{
		if(state >= DSS_COUNT)
			return false;

		Renderer::theContextPtr->OMSetDepthStencilState(dsStates[state], stencilRefs[state]);
		currentState = state;
		return true;
	}

	void DepthStencilStateManager::CreateStates()
	{
		D3D11_DEPTH_STENCIL_DESC dssDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		dssDesc.DepthEnable = FALSE;

		Renderer::theDevicePtr->CreateDepthStencilState(&dssDesc, &dsStates[DSS_NoDepth].p);
		stencilRefs[DSS_NoDepth] = 0;

		dssDesc.DepthEnable = TRUE;
		Renderer::theDevicePtr->CreateDepthStencilState(&dssDesc, &dsStates[DSS_Default].p);
		stencilRefs[DSS_Default] = 0;

		dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		Renderer::theDevicePtr->CreateDepthStencilState(&dssDesc, &dsStates[DSS_LessEqual].p);
		stencilRefs[DSS_LessEqual] = 0;

		dssDesc.DepthFunc = D3D11_COMPARISON_GREATER;
		dssDesc.StencilEnable = TRUE;
		dssDesc.FrontFace.StencilPassOp = 
			dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		Renderer::theDevicePtr->CreateDepthStencilState(&dssDesc, &dsStates[DSS_DeferredLight1].p);
		stencilRefs[DSS_DeferredLight1] = 0x1;

		dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dssDesc.FrontFace.StencilFunc = 
			dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		dssDesc.FrontFace.StencilPassOp = 
			dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		Renderer::theDevicePtr->CreateDepthStencilState(&dssDesc, &dsStates[DSS_DeferredLight2].p);
		stencilRefs[DSS_DeferredLight2] = 0x1;

		dssDesc.FrontFace.StencilPassOp = 
			dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

		//dssDesc.FrontFace.StencilFunc = 
		//	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

		//dssDesc.FrontFace.StencilFailOp = 
		//	dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		dssDesc.DepthEnable = FALSE;
		Renderer::theDevicePtr->CreateDepthStencilState(&dssDesc, &dsStates[DSS_LightOutsideFinal].p);
		stencilRefs[DSS_LightOutsideFinal] = 0x2;

		Renderer::theDevicePtr->CreateDepthStencilState(&dssDesc, &dsStates[DSS_LightInsideFinal].p);
		stencilRefs[DSS_LightInsideFinal] = 0x1;
	}
}