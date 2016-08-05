#include "ToolRenderController.h"

#include "../EDRendererD3D/Renderer.h"
#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;
using namespace EDRendererD3D;

namespace FSGDEngineToolInterface
{
	ToolRenderController *ToolRenderController::instancePtr = nullptr;

	ToolRenderController::ToolRenderController(void)
	{
	}


	ToolRenderController::~ToolRenderController(void)
	{
	}

	ToolRenderController *ToolRenderController::GetInstance(void)
	{
		if(nullptr == instancePtr)
		{
			instancePtr = new ToolRenderController;
		}
		return instancePtr;
	}

	ToolRenderController &ToolRenderController::GetReference(void)
	{
		return *GetInstance();
	}

	void ToolRenderController::DeleteInstance(void)
	{
		delete instancePtr;
	}

	void ToolRenderController::Initialize(HWND hWnd)
	{
		EDRendererD3D::Renderer::Initialize(hWnd);

		CreateContexts();
	}

	void ToolRenderController::Render(void)
	{
		const FLOAT clearColor[4] = { 0, 1, 1, 1 };
		EDRendererD3D::Renderer::ClearRenderTarget(clearColor);
		EDRendererD3D::Renderer::ClearDepthAndStencilTarget();



		EDRendererD3D::Renderer::Present();
	}

	void ToolRenderController::Shutdown(void)
	{
		EDRendererD3D::Renderer::Shutdown();
		twoDContext.Release();
		directionDeferredContextHandle.Release();
		pointLightContextHandle.Release();
		spotLightContextHandle.Release();
		debugLines.Release();
		debugTriangles.Release();
	}

	void ToolRenderController::CreateContexts(void)
	{
		twoDContext = ContentManager::LoadXML<RenderContext>( 
			"GDRenderContext/rcx_TwoDPassThrough.xml");

		directionDeferredContextHandle = ContentManager::LoadXML<RenderContext>( 
			"GDRenderContext/rcx_DirectionalLightDeferred.xml");
		directionDeferredContextHandle.GetContent()->SetRenderStage( RenderContext::RS_DIR_LIGHT );

		pointLightContextHandle = ContentManager::LoadXML<RenderContext>( "GDRenderContext/rcx_PointLightDeferred.xml" );
		pointLightContextHandle.GetContent()->SetRenderStage( RenderContext::RS_PNT_LIGHT );

		spotLightContextHandle = ContentManager::LoadXML<RenderContext>( 
			"GDRenderContext/rcx_SpotLightDeferred.xml" );
		spotLightContextHandle.GetContent()->SetRenderStage( RenderContext::RS_SPT_LIGHT );

		debugLines = ContentManager::LoadXML<RenderContext>("GDRenderContext/rcx_DebugLines.xml");
		AddRenderContext(debugLines.GetContent());

		debugTriangles = ContentManager::LoadXML<RenderContext>("GDRenderContext/rcx_DebugTriangles.xml");
		AddRenderContext(debugTriangles.GetContent());
	}

	void ToolRenderController::AddRenderContext(RenderContext* pContext)
	{
		unsigned int pTechniqueValue = 0;
		std::list< RenderContext*>::iterator listIter;

		pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_DEPTH_SPT_LIGHT );
		pTechniqueValue |= (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_DEPTH_PNT_LIGHT );
		pTechniqueValue |= (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_DEPTH_DIR_LIGHT );

		for( listIter = shadowCasters.begin(); listIter != shadowCasters.end(); ++listIter )
		{
			if( (*listIter) == pContext )
			{
				pTechniqueValue = 0;
				break;
			}
		}
		if( pTechniqueValue != 0 )
			shadowCasters.push_back( pContext );

		pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_GBUFFER );

		for( listIter = gBufferContexts.begin(); listIter != gBufferContexts.end(); ++listIter )
		{
			if( (*listIter) == pContext )
			{
				pTechniqueValue = 0;
				break;
			}
		}

		if( pTechniqueValue != 0 )
			gBufferContexts.push_back( pContext );

		pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_DEPTH_CAMERA );

		for( listIter = depthCameraContexts.begin(); listIter != depthCameraContexts.end(); ++listIter )
		{
			if( (*listIter) == pContext )
			{
				pTechniqueValue = 0;
				break;
			}
		}
		if( pTechniqueValue != 0 )
			depthCameraContexts.push_back( pContext );

		pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_UNLIT );

		for( listIter = unlitOpaqueContexts.begin(); listIter != unlitOpaqueContexts.end(); ++listIter )
		{
			if( (*listIter) == pContext )
			{
				pTechniqueValue = 0;
				break;
			}
		}
		if( pTechniqueValue != 0 )
			unlitOpaqueContexts.push_back( pContext );

		pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_GUI );

		pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_TRANSPARENT );

		if( pTechniqueValue != 0 )
		{
			transparentContextPtr = pContext;
		}
	}

}