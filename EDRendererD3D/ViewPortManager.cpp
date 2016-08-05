#include "precompiled.h"
#include "ViewPortManager.h"

using namespace EDMath;

namespace EDRendererD3D
{
	ViewPortManager *ViewPortManager::instancePtr = 0;

	ViewPortManager &ViewPortManager::GetReference()
	{
		if(!instancePtr)
			instancePtr = new ViewPortManager;
		return *instancePtr;
	}

	void ViewPortManager::DeleteInstance()
	{
		delete instancePtr;
		instancePtr = 0;
	}

	ViewPortManager::ViewPortManager(void)
	{
		activePort = 0;
	}

	ViewPortManager::~ViewPortManager(void)
	{
	}

	void ViewPortManager::AddViewPort(RenderMesh *_renderMeshPtr, RenderContext *_renderContextPtr, 
		RenderTarget *_renderTargetPtr, const Float4x4 &matrix)
	{
		viewPorts.push_back(ViewPort());
		viewPorts.back().viewPortQuad.Initialize(_renderMeshPtr, _renderContextPtr, _renderTargetPtr);
		viewPorts.back().viewPortQuad.RenderFunc = RenderShapeTarget::IndexedPrimitiveNoTransformRenderFunc;
//		viewPorts.back().viewPortQuad.SetWorldMatrix( (D3DXMATRIX *)&matrix);
		viewPorts.back().view.makeIdentity();
		viewPorts.back().projection.makeIdentity();
	}

	void ViewPortManager::RenderActiveViewPort()
	{
		viewPorts[activePort].viewPortQuad.AddToContextSet();
		viewPorts[activePort].viewPortQuad.GetRenderContext()->RenderProcess();
		viewPorts[activePort].viewPortQuad.GetRenderContext()->ClearRenderSet();
	}

	void ViewPortManager::UpdateViewPort(int port, const Float4x4 &_world, const Float4x4 &_view, const Float4x4 &_projection, float _nearClip, float _farClip)
	{
		viewPorts[port].world = _world;
		viewPorts[port].view = _view;
		viewPorts[port].projection = _projection;
		viewPorts[port].nearClip = _nearClip;
		viewPorts[port].farClip = _farClip;

		Multiply(viewPorts[port].viewProjection, viewPorts[port].view, viewPorts[port].projection);
	}
	void ViewPortManager::UpdateActiveViewPort(const Float4x4 &_world, const Float4x4 &_view, const Float4x4 &_projection, float _nearClip, float _farClip)
	{
		UpdateViewPort(activePort, _world, _view, _projection, _nearClip, _farClip);
	}
}
