#include "precompiled.h"
#include "VisibilityQueryInterface.h"
#include "Renderer.h"

namespace EDRendererD3D
{
	RenderShapeBase *VisibilityQueryInterface::theShapePtr = 0;

	VisibilityQueryInterface::VisibilityQueryInterface(void)
	{
//		queryPtr = 0;
		//depthSurfacePtr = 0;
		renderTargetPtr = 0;
		pixelCountThreshold = 0;
	}


	VisibilityQueryInterface::~VisibilityQueryInterface(void)
	{
		//queryPtr->Release();
	}

	//void VisibilityQueryInterface::Initialize(
	//	RenderTarget *_renderTargetPtr, IDirect3DSurface9 *_depthSurfacePtr)
	//{
	//	depthSurfacePtr = _depthSurfacePtr;
	//	renderTargetPtr = _renderTargetPtr;

	//	Renderer::theDevicePtr->CreateQuery(D3DQUERYTYPE_OCCLUSION, &queryPtr);
	//}

	//void VisibilityQueryInterface::IssueQuery(D3DXMATRIX *transformMat)
	//{
	//	// Begin the query
	//	queryPtr->Issue(D3DISSUE_BEGIN);

	//	// Render the object
	//	LPDIRECT3DSURFACE9 pOldDS = NULL;
	//	Renderer::theDevicePtr->GetDepthStencilSurface( &pOldDS );
	//	Renderer::theDevicePtr->SetDepthStencilSurface( depthSurfacePtr );
	//	//renderTargetPtr->ActivateTarget(0);
	//	//Renderer::Clear(0,0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1, 0); 

	//	assert(theShapePtr);
	//	theShapePtr->SetWorldMatrix(transformMat);
	//	theShapePtr->GetRenderContext()->AddRenderNode(theShapePtr);
	//	theShapePtr->GetRenderContext()->RenderProcess();
	//	theShapePtr->GetRenderContext()->ClearRenderSet();

	//	//renderTargetPtr->RevertTarget();
	//	if( pOldDS )
	//	{
	//		Renderer::theDevicePtr->SetDepthStencilSurface( pOldDS );
	//		pOldDS->Release();
	//	}
	//	// End the query
	//	queryPtr->Issue(D3DISSUE_END);
	//}

	VisibilityQueryResult VisibilityQueryInterface::GetResult(void)
	{
		VisibilityQueryResult retVal = VQR_Incomplete;
		//DWORD visiblePixels = 0;
		//if(S_FALSE == queryPtr->GetData(&visiblePixels, sizeof(DWORD), D3DGETDATA_FLUSH))
		//	retVal = VQR_Incomplete;
		//else
		//{
		//	if(visiblePixels > pixelCountThreshold)
		//		retVal = VQR_Visible;
		//	else
		//		retVal = VQR_NotVisiable;
		//}

		return retVal;
	}
}
