#include "precompiled.h"
#include "../FSGDGame/PointLightSource.h"
#include "../FSGDGame/DirectionalLightSource.h"
#include "../FSGDGame/SpotLightSource.h"
#include "../FSGDGame/FSGDGame.h"

#include "../FSGDGame/RenderController.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/DepthStencilStateManager.h"
#include "../EDRendererD3D/BlendStateManager.h"
#include "../EDRendererD3D/RasterizerStateManager.h"
#include "../EDRendererD3D/GraphicsProfiler.h"
using namespace EDRendererD3D;
#include "../EDUtilities/PrintConsole.h"
using EDUtilities::PrintConsole;
using EDMath::Float3;
using std::list;
using std::set;
using namespace EDUtilities;

extern GameBase *globalGamePtr;


void RenderController::RenderGBuffersSolution()
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Deferred Shading", Float3(1.0f, 0.0f, 0.0f), 
			L"RenderController::RenderGBuffersSolution");
		added = true;
	}
	//Switch render targets
	gBufferRT.ActivateTarget();

	const FLOAT clearColor[4] = { 0, 1, 0, 1 };
	gBufferRT.ClearRenderTargetView(clearColor);
	gBufferRT.ClearDepthStencilView(D3D11_CLEAR_DEPTH);

	// Render g-buffers
	for(list<RenderContext * >::iterator iter = 
		gBufferContexts.begin(); iter != gBufferContexts.end(); ++iter)
	{
		(*iter)->SetRenderStage(RenderContext::RS_GBUFFER);
		(*iter)->RenderProcess();
	}
}

void RenderController::RenderDirectionalLightSolution(DirectionalLightSource *lightSourcePtr,
	const EDMath::Aabb *cascadeAabbs, unsigned int numCascades)
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Deferred Shading", Float3(1.0f, 0.0f, 0.0f), 
			L"RenderController::RenderDirectionalLightSolution");
		added = true;
	}

	DepthStencilStateManager::GetReference().ApplyState(DepthStencilStateManager::DSS_NoDepth);

	BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Additive);

	gBufferRT.ActivateSRVs(GBufferStartSlot);

	DirectionalLightWithShadow *lightPtr =
		static_cast<DirectionalLightWithShadow *>(lightSourcePtr->GetLightPtr());

	for (unsigned int i = 0; i < numCascades; ++i)
	{
		GraphicsProfiler::GetReference().BeginEvent(0, L"Render Dir Shadows 0");
		BuildDirViewProjectionCascade(cascadeAabbs[i], lightPtr, i);
		RenderDirectionalDepths(lightPtr, i);
		GraphicsProfiler::GetReference().EndEvent();
	}
	sceneTarget.ActivateTarget();
	sceneTarget.ClearDepthStencilView(D3D11_CLEAR_STENCIL);

	GraphicsProfiler::GetReference().BeginEvent(0, L"Render a directional light with shadow begin");
	lightPtr->ActivateShadowTexture();
	directionDeferredContextHandle.GetContent()->RenderProcess();
	lightPtr->RevertShadowTexture();
	GraphicsProfiler::GetReference().EndEvent();

	directionDeferredContextHandle.GetContent()->ClearRenderSet();
}

void RenderController::RenderDirectionalLightNoCascadeSolution(
	DirectionalLightWithShadow *lightPtr,	const EDMath::Aabb &aabb)
{
	DepthStencilStateManager::GetReference().ApplyState(DepthStencilStateManager::DSS_NoDepth);

	BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Additive);

	gBufferRT.ActivateSRVs(GBufferStartSlot);

	BuildDirViewProjectionCascade(aabb, lightPtr, 0);

	RenderDirectionalDepths(lightPtr, 0);
	sceneTarget.ActivateTarget();
	sceneTarget.ClearDepthStencilView(D3D11_CLEAR_STENCIL);

	GraphicsProfiler::GetReference().BeginEvent(0, L"Render a directional light with shadow begin");
	lightPtr->ActivateShadowTexture();
	directionDeferredContextHandle.GetContent()->RenderProcess();
	lightPtr->RevertShadowTexture();
	GraphicsProfiler::GetReference().EndEvent();

	directionDeferredContextHandle.GetContent()->ClearRenderSet();
}

void RenderController::RenderPointLightSolution(PointLightSource *lightSourcePtr)
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Deferred Shading", Float3(1.0f, 0.0f, 0.0f), 
			L"RenderController::RenderPointLightSolution");
		added = true;
	}

	DepthStencilStateManager::GetReference().ApplyState(DepthStencilStateManager::DSS_NoDepth);
	
	BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Additive);
	
	gBufferRT.ActivateSRVs(GBufferStartSlot);

	if(EDGameCore::ILight::SOFT == lightSourcePtr->GetShadows())
	{
		PointLightWithShadow *lightPtr = 
			static_cast<PointLightWithShadow *>(lightSourcePtr->GetLightPtr());

		RenderPointDepths(lightPtr);

		sceneTarget.ActivateTarget();
		sceneTarget.ClearDepthStencilView(D3D11_CLEAR_STENCIL);

		GraphicsProfiler::GetReference().BeginEvent(0, L"Render a point light with shadow begin");
		lightPtr->ActivateShadowTexture();
		pointLightWithShadowsContextHandle.GetContent()->RenderProcess();
		lightPtr->RevertShadowTexture();
		GraphicsProfiler::GetReference().EndEvent();

		pointLightWithShadowsContextHandle.GetContent()->ClearRenderSet();
	}
	else
	{		
		PointLight *lightPtr = 
			static_cast<PointLight *>(lightSourcePtr->GetLightPtr());

		sceneTarget.ActivateTarget();
		sceneTarget.ClearDepthStencilView(D3D11_CLEAR_STENCIL);
		lightPtr->ApplyLight();

		GraphicsProfiler::GetReference().BeginEvent(0, L"Render a point light");
		pointLightContextHandle.GetContent()->RenderProcess();
		GraphicsProfiler::GetReference().EndEvent();

		pointLightContextHandle.GetContent()->ClearRenderSet();
	}
}

void RenderController::RenderSpotLightSolution(SpotLightSource *lightSourcePtr)
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Deferred Shading", Float3(1.0f, 0.0f, 0.0f), 
			L"RenderController::RenderSpotLightSolution");
		added = true;
	}

	DepthStencilStateManager::GetReference().ApplyState(DepthStencilStateManager::DSS_NoDepth);
	
	BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Additive);
	
	gBufferRT.ActivateSRVs(GBufferStartSlot);

	if(EDGameCore::ILight::SOFT == lightSourcePtr->GetShadows())
	{
		SpotLightWithShadow *lightPtr = 
			static_cast<SpotLightWithShadow *>(lightSourcePtr->GetLightPtr());

		RenderSpotDepths(lightPtr);

		sceneTarget.ActivateTarget();
		sceneTarget.ClearDepthStencilView(D3D11_CLEAR_STENCIL);

		GraphicsProfiler::GetReference().BeginEvent(0, L"Render a spot light with shadow begin");
		lightPtr->ActivateShadowTexture();
		spotLightWithShadowsContextHandle.GetContent()->RenderProcess();
		lightPtr->RevertShadowTexture();
		GraphicsProfiler::GetReference().EndEvent();

		spotLightWithShadowsContextHandle.GetContent()->ClearRenderSet();
	}
	else
	{		
		SpotLight *lightPtr = 
			static_cast<SpotLight *>(lightSourcePtr->GetLightPtr());
		lightPtr->ApplyLight();

		sceneTarget.ActivateTarget();
		sceneTarget.ClearDepthStencilView(D3D11_CLEAR_STENCIL);

		GraphicsProfiler::GetReference().BeginEvent(0, L"Render a spot light");
		spotLightContextHandle.GetContent()->RenderProcess();
		GraphicsProfiler::GetReference().EndEvent();

		spotLightContextHandle.GetContent()->ClearRenderSet();
	}
}

void RenderController::RenderUnlitObjectsSolution()
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Deferred Shading", Float3(1.0f, 0.0f, 0.0f), 
			L"RenderController::RenderUnlitObjectsSolution");
		added = true;
	}

	GraphicsProfiler::GetReference().BeginEvent(0, L"Render unlit begin");

	DepthStencilStateManager::GetReference().ApplyState(DepthStencilStateManager::DSS_LessEqual);
	gBufferRT.DeactivateSRVs(GBufferStartSlot);

	BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Default);
	std::list< RenderContext*>::iterator listIter = unlitOpaqueContexts.begin();
	for(; listIter != unlitOpaqueContexts.end(); ++listIter)
	{
		(*listIter)->SetRenderStage( RenderContext::RS_UNLIT );
		(*listIter)->RenderProcess();
		(*listIter)->ClearRenderSet();
	}

	DepthStencilStateManager::GetReference().ApplyState(DepthStencilStateManager::DSS_Default);
	GraphicsProfiler::GetReference().EndEvent();
}

void RenderController::RenderTransparentObjectsSolution()
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Deferred Shading", Float3(1.0f, 0.0f, 0.0f), 
			L"RenderController::RenderTransparentObjectsSolution");
		added = true;
	}

	GraphicsProfiler::GetReference().BeginEvent(0, L"Render transparent begin");

	transSceneTarget.ActivateTarget();

	BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Alpha);
	if( transparentContextPtr != 0 )
	{
		transparentContextPtr->SetRenderStage( RenderContext::RS_TRANSPARENT );
		transparentContextPtr->RenderProcess();
	}

	BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Default);	
	sceneTarget.ActivateTarget();
	GraphicsProfiler::GetReference().EndEvent();
}

void RenderController::RenderDebugBuffers()
{
	if(shouldRenderDebugBuffer)
	{
		if(GB_Depth == debugBufferToRender)
			debugBufferShape.SetRenderTarget(&gBufferRT, 3);
		else if(GB_Diffuse == debugBufferToRender)
			debugBufferShape.SetRenderTarget(&gBufferRT, 0);
		else if(GB_Normal == debugBufferToRender)
			debugBufferShape.SetRenderTarget(&gBufferRT, 1);
		else if(GB_Specular == debugBufferToRender)
			debugBufferShape.SetRenderTarget(&gBufferRT, 2);
		else if(GB_DirShadow0 == debugBufferToRender)
			debugBufferShape.SetRenderTarget(&dirLightPtr->GetRenderTarget(0));
		else if (GB_DirShadow1 == debugBufferToRender)
			debugBufferShape.SetRenderTarget(&dirLightPtr->GetRenderTarget(1));
		else if (GB_DirShadow2 == debugBufferToRender)
			debugBufferShape.SetRenderTarget(&dirLightPtr->GetRenderTarget(2));
			//debugBufferShape.SetRenderTarget(&RenderController::GetInstance()->GetSpotShadowRenderTarget());
		debugBufferShape.AddToContextSet();
	}
}

void RenderController::InitializeDebugBuffers()
{
	quarterScreenQuadMesh = RenderMesh::LoadXMLObject("GDMesh/ScreenQuad/QuarterScreenQuadShape.xml", "VERTEX_POSTEX");
	debugBufferShape.Initialize(&quarterScreenQuadMesh, twoDContext.GetContent(), 0);
	debugBufferShape.RenderFunc = RenderShapeTarget::IndexedPrimitiveNoTransformRenderFunc;
	debugBufferShape.SetRenderTarget(&gBufferRT);
}
