
#include "RenderController.h"
#include "WorldData.h"
#include "PostProcessController.h"
#include "PointLightSource.h"
#include "SpotLightSource.h"
#include "DirectionalLightSource.h"

#include "../EDRendererD3D/ConstantBuffers.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/ViewPortManager.h"
#include "../EDRendererD3D/TextRenderer.h"
#include "../EDRendererD3D/DepthStencilStateManager.h"
#include "../EDRendererD3D/RasterizerStateManager.h"
#include "../EDRendererD3D/BlendStateManager.h"
#include "../EDRendererD3D/GraphicsProfiler.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDEventManager/EventManager.h"
#include "../EDInput/InputManager.h"
#include "../EDRendererD3D/RenderSpriteManager.h"
#include "../EDUtilities/Settings.h"

#include "../EDTerrain/TerrainManager.h"
using namespace EDTerrain;
using std::list;
using namespace DirectX;
using namespace EDMath;
using namespace EDUtilities;
using namespace EDRendererD3D;

RenderController *RenderController::instancePtr = 0;

int RenderController::thePointShadowSize = 256;
int RenderController::theDirectionalShadowSize = 2048;
int RenderController::theSpotShadowSize = 256;

RenderController *RenderController::GetInstance()
{
	if( instancePtr == 0 )
		instancePtr = new RenderController;

	return instancePtr;
}

void RenderController::DeleteInstance(void)
{
	delete instancePtr;
	instancePtr = 0;
}

void OnRenderDepthCallback(unsigned int eventId, const void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_Depth);
}
void OnRenderDiffuseCallback(unsigned int eventId, const void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_Diffuse);
}
void OnRenderNormalCallback(unsigned int eventId, const void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_Normal);
}
void OnRenderSpecularCallback(unsigned int eventId, const void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_Specular);
}
void OnRenderDirShadow0Callback(unsigned int eventId, const void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_DirShadow0);
}
void OnRenderDirShadow1Callback(unsigned int eventId, const void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_DirShadow1);
}
void OnRenderDirShadow2Callback(unsigned int eventId, const void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_DirShadow2);
}


RenderController::RenderController() 
{
	transparentContextPtr = 0;

	screenQuadMeshPtr = 0;
	shouldRenderDebugBuffer = false;
	debugBufferToRender = 0;
	firstResize = true;

	spriteBatchPtr = nullptr;
	spriteFontPtr = nullptr;
	VisibilityQueryInterface::theShapePtr = 0;
}

RenderController::~RenderController()
{
	if(screenQuadMeshPtr)
		delete screenQuadMeshPtr;
	if(VisibilityQueryInterface::theShapePtr)
		delete VisibilityQueryInterface::theShapePtr;

	delete spriteBatchPtr;
	delete spriteFontPtr;

	//TerrainManager::DeleteInstance();
	DepthStencilStateManager::DeleteInstance();
	RasterizerStateManager::DeleteInstance();
	BlendStateManager::DeleteInstance();
	GraphicsProfiler::DeleteInstance();
	RenderSpriteManager::DeleteInstance();
	PostProcessController::DeleteInstance();
}

void RenderController::Initialize()
{
	CreateContexts();
	CreateDepthBuffers();
	InitializeConsoleCommands();
	CreateViewPorts(1);

	//CreateRenderTargets();
	CreatePostProcessEffects();
	CreateConstantBuffers();

	//InitializeTextureSamplers();

	forwardLightManager.Initialize();

	using namespace EDInput;

	AddInputCallback(KEYBOARD_TYPE, '1', INPUT_PRESSED, "RenderDepths", OnRenderDepthCallback);
	AddInputCallback(KEYBOARD_TYPE, '2', INPUT_PRESSED, "RenderDiffuse", OnRenderDiffuseCallback);
	AddInputCallback(KEYBOARD_TYPE, '3', INPUT_PRESSED, "RenderNormals", OnRenderNormalCallback);
	AddInputCallback(KEYBOARD_TYPE, '4', INPUT_PRESSED, "RenderSpecular", OnRenderSpecularCallback);
	AddInputCallback(KEYBOARD_TYPE, '5', INPUT_PRESSED, "RenderShadow0", OnRenderDirShadow0Callback);
	AddInputCallback(KEYBOARD_TYPE, '6', INPUT_PRESSED, "RenderShadow1", OnRenderDirShadow1Callback);
	AddInputCallback(KEYBOARD_TYPE, '7', INPUT_PRESSED, "RenderShadow2", OnRenderDirShadow2Callback);


	// Font stuf...
	spriteBatchPtr = new SpriteBatch(Renderer::theContextPtr);
	spriteFontPtr = new SpriteFont(Renderer::theDevicePtr, L"Resources/GDAssets/FontData/Calibri.spritefont");

	ViewPortManager &viewPortMan = ViewPortManager::GetReference();
	viewPortMan.SetActiveViewPort(0);

	// Read from the setting file the desired shadow map size
	EDUtilities::Settings::GetInstance()->GetSetting("DirectionalShadowSize", 
		theDirectionalShadowSize, theDirectionalShadowSize);
	EDUtilities::Settings::GetInstance()->GetSetting("PointShadowSize", 
		thePointShadowSize, thePointShadowSize);
	EDUtilities::Settings::GetInstance()->GetSetting("SpotShadowSize", 
		theSpotShadowSize, theSpotShadowSize);

}

void RenderController::UpdateResolution()
{
	InitializeTextureSamplers();
	CreateRenderTargets();

	PostProcessController::GetInstance()->UpdateResolution();
}

void RenderController::CreateContexts()
{
	twoDContext = ContentManager::LoadXML<RenderContext>( 
		"GDRenderContext/rcx_TwoDPassThrough.xml");

	directionDeferredContextHandle = ContentManager::LoadXML<RenderContext>( 
		"GDRenderContext/rcx_DirectionalLightDeferred.xml");
	directionDeferredContextHandle.GetContent()->SetRenderStage( RenderContext::RS_DIR_LIGHT );

	pointLightContextHandle = ContentManager::LoadXML<RenderContext>( 
		"GDRenderContext/rcx_PointLightDeferred.xml" );
	pointLightContextHandle.GetContent()->SetRenderStage( RenderContext::RS_PNT_LIGHT );

	pointLightWithShadowsContextHandle = ContentManager::LoadXML<RenderContext>( 
		"GDRenderContext/rcx_PointLightWithShadowsDeferred.xml" );
	pointLightWithShadowsContextHandle.GetContent()->SetRenderStage( RenderContext::RS_PNT_LIGHT );

	spotLightContextHandle = ContentManager::LoadXML<RenderContext>( 
		"GDRenderContext/rcx_SpotLightDeferred.xml" );
	spotLightContextHandle.GetContent()->SetRenderStage( RenderContext::RS_SPT_LIGHT );

	spotLightWithShadowsContextHandle = ContentManager::LoadXML<RenderContext>( 
		"GDRenderContext/rcx_SpotLightWithShadowsDeferred.xml" );
	spotLightWithShadowsContextHandle.GetContent()->SetRenderStage( RenderContext::RS_SPT_LIGHT );

	debugLines = ContentManager::LoadXML<RenderContext>("GDRenderContext/rcx_DebugLines.xml");
	AddRenderContext(debugLines.GetContent());

	debugTriangles = ContentManager::LoadXML<RenderContext>("GDRenderContext/rcx_DebugTriangles.xml");
	AddRenderContext(debugTriangles.GetContent());

	//TerrainManager::GetReference().Initialize();
}

void RenderController::AddRenderContext(RenderContext* pContext)
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

	for( listIter = guiContexts.begin(); listIter != guiContexts.end(); ++listIter )
	{
		if( (*listIter) == pContext )
		{
			pTechniqueValue = 0;
			break;
		}
	}
	if( pTechniqueValue != 0 )
		guiContexts.push_back( pContext );

	pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_TRANSPARENT );

	if( pTechniqueValue != 0 )
	{
		transparentContextPtr = pContext;
	}
}

void RenderController::CreateRenderTargets()
{
	UINT width = Renderer::GetResolutionWidth();
	UINT height = Renderer::GetResolutionHeight();
	// Set up the scene targets
	sceneTarget.Create(Renderer::theDepthStencilViewPtr, Renderer::theDepthStencilBufferPtr);
	//sceneTargetView.Create(width, height,
		sceneTargetView.Create(width, height,
		DXGI_FORMAT_R8G8B8A8_UNORM, "Scene Target");
	sceneTarget.AddTarget(&sceneTargetView);

	gBufferRT.Create( Renderer::theDepthStencilViewPtr, Renderer::theDepthStencilBufferPtr);
	// Diffuse
	//gBufferDiffuseTargetView.Create(width, height,
	gBufferDiffuseTargetView.Create(width, height,
		DXGI_FORMAT_R8G8B8A8_UNORM, "GBufferDiffuse");
	gBufferRT.AddTarget(&gBufferDiffuseTargetView);
	// Normals
	//gBufferNormalsTargetView.Create(width, height,
	gBufferSpecularTargetView.Create(width, height,
		DXGI_FORMAT_R10G10B10A2_UNORM, "GBufferSpecular");
	gBufferRT.AddTarget(&gBufferSpecularTargetView);
	gBufferNormalsTargetView.Create(width, height,
		//DXGI_FORMAT_R8G8B8A8_UNORM, "GBufferNormals");
		DXGI_FORMAT_R10G10B10A2_UNORM, "GBufferNormals");
	gBufferRT.AddTarget(&gBufferNormalsTargetView);
	// Specular
	//gBufferSpecularTargetView.Create(width, height,

	// Depths
	//gBufferDepthTargetView.Create(width, height,
	gBufferDepthTargetView.Create(width, height,
		DXGI_FORMAT_R32_FLOAT, "GBufferDepths");
	gBufferRT.AddTarget(&gBufferDepthTargetView);


	transSceneTarget.Create(Renderer::theDepthStencilViewPtr, Renderer::theDepthStencilBufferPtr);
	transSceneTarget.AddTarget(&sceneTargetView);
	transSceneTarget.AddTarget(&gBufferNormalsTargetView);
	transSceneTarget.AddTarget(&gBufferDepthTargetView);

}

void RenderController::CreateDepthBuffers()
{
	spotDepths.Create(theSpotShadowSize, theSpotShadowSize);
	spotDepthsTargetView.Create(theSpotShadowSize, theSpotShadowSize, 
		DXGI_FORMAT_R32_FLOAT, "Spot Depth Target");
	spotDepths.AddTarget(&spotDepthsTargetView);
}

void RenderController::CreateConstantBuffers()
{
	neverChangeCBuffer.Release();
	cameraCBuffer.Release();

    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(cbMisc);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );

	float width = (float)Renderer::GetResolutionWidth();
	float height = (float)Renderer::GetResolutionHeight();

	miscBuffer.gHalfPixel = XMFLOAT2(0.5f / width, 0.5f / height);

	float shadowBias = 0.002f;
	EDUtilities::Settings::GetInstance()->GetSetting("ShadowBias", shadowBias, shadowBias);

	miscBuffer.gShadowDepthOffset = shadowBias;
	miscBuffer.gShadowDirOffset = 1.0f / (theDirectionalShadowSize*2.0f);

	miscBuffer.gDisplacementScale = 0.5f;

	miscBuffer.gMaxTessDistance = 25.0f;
	miscBuffer.gMinTessDistance = 1.0f;
	miscBuffer.gMaxTessFactor	= 24.0f;
	miscBuffer.gMinTessFactor	= 1.0f;

	InitData.pSysMem = &miscBuffer;

	HR(Renderer::theDevicePtr->CreateBuffer(&bd, &InitData, &neverChangeCBuffer));
	
	Renderer::theContextPtr->VSSetConstantBuffers(miscBuffer.REGISTER_SLOT, 1, &neverChangeCBuffer.p);
	Renderer::theContextPtr->PSSetConstantBuffers(miscBuffer.REGISTER_SLOT, 1, &neverChangeCBuffer.p);
	Renderer::theContextPtr->GSSetConstantBuffers(miscBuffer.REGISTER_SLOT, 1, &neverChangeCBuffer.p);
	Renderer::theContextPtr->HSSetConstantBuffers(miscBuffer.REGISTER_SLOT, 1, &neverChangeCBuffer.p);
	Renderer::theContextPtr->DSSetConstantBuffers(miscBuffer.REGISTER_SLOT, 1, &neverChangeCBuffer.p);

	// The camera cbuffer
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(cbPerCamera);
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &cameraCBuffer));

	//// Initialize post process constant buffer values
	//postShaderData.gTexCoordScale = 1;
	//postShaderData.gFocusDistance = 15.0f;
	//postShaderData.gFocusRange = 50.0f;
	//postShaderData.gEdgeWidth = 1.0f;
	//postShaderData.gEdgeIntensity = 5.0f;

	//CComPtr<ID3D11Buffer> postShaderDataCBuffer;

	//ZeroMemory( &bd, sizeof(bd) );
	//bd.Usage = D3D11_USAGE_DYNAMIC;
	//bd.ByteWidth = sizeof(cbPostProcess);
	//bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

 //   ZeroMemory( &InitData, sizeof(InitData) );
	//InitData.pSysMem = &postShaderData;
	//HR(Renderer::theDevicePtr->CreateBuffer(&bd, &InitData, &postShaderDataCBuffer));

	//Renderer::theContextPtr->VSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);
	//Renderer::theContextPtr->PSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);
	//Renderer::theContextPtr->GSSetConstantBuffers(cbPostProcess::REGISTER_SLOT, 1, &postShaderDataCBuffer.p);
}

void RenderController::CreateViewPorts(UINT viewPorts)
{
	delete screenQuadMeshPtr;
	screenQuadMeshPtr = RenderMesh::LoadXML("GDMesh/ScreenQuad/ScreenQuadShape.xml", "VERTEX_POSTEX");
	screenQuad.Initialize(screenQuadMeshPtr, twoDContext.GetContent(), 0);
	screenQuad.RenderFunc = RenderShapeTarget::IndexedPrimitiveNoTransformRenderFunc;

	// Creates data to be used for debug rendering of additional buffers
	InitializeDebugBuffers();

	Float4x4 viewPortMat;
	if(1 == viewPorts)
	{
		viewPortMat.makeIdentity();
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, twoDContext.GetContent(),
			&sceneTarget,	viewPortMat);
	}
	else if(2 == viewPorts)
	{
		viewPortMat.makeScale(1.0f, .5f, 1.0f);
		viewPortMat.translateGlobal(0.0f, 0.5f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, twoDContext.GetContent(),
			&sceneTarget, viewPortMat);

		viewPortMat.translateGlobal(0.0f, -1.0f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, twoDContext.GetContent(),
			&sceneTarget, viewPortMat);
	}
	else if(4 == viewPorts || 3 == viewPorts)
	{
		viewPortMat.makeScale(0.5f, .5f, 1.0f);
		viewPortMat.translateGlobal(-.5f, 0.5f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, twoDContext.GetContent(),
			&sceneTarget, viewPortMat);

		viewPortMat.translateGlobal(1.0f, 0.0f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, twoDContext.GetContent(),
			&sceneTarget, viewPortMat);

		viewPortMat.translateGlobal(-1.0f, -1.0f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, twoDContext.GetContent(),
			&sceneTarget, viewPortMat);

		viewPortMat.translateGlobal(1.0f, 0.0f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, twoDContext.GetContent(),
			&sceneTarget, viewPortMat);
	}
}

void RenderController::CreatePostProcessEffects()
{
	PostProcessController::GetInstance()->InitializePostEffects(screenQuadMeshPtr);
}

float CalcGaussianBlurWeight(float x)
{
	const float StandardDeviation = 1;
	const float StandardDeviationSquared = 
		StandardDeviation * StandardDeviation;

	return (1.0f / sqrt(2 * (float)M_PI * StandardDeviationSquared)) * 
		pow(float(M_E), float(-((x * x) / (2 * StandardDeviationSquared))));
}

void RenderController::InitializeTextureSamplers()
{
	CComPtr<ID3D11SamplerState> samplers[7];
	D3D11_SAMPLER_DESC desc;
	//anisoWrapSampler
	desc.Filter = D3D11_FILTER_ANISOTROPIC;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = 4;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;
	HR(Renderer::theDevicePtr->CreateSamplerState(&desc, &samplers[0].p));

	//anisoClampSampler
	desc.Filter = D3D11_FILTER_ANISOTROPIC;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MaxAnisotropy = 4;
	HR(Renderer::theDevicePtr->CreateSamplerState(&desc, &samplers[1].p));

	//pointWrapSampler
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	HR(Renderer::theDevicePtr->CreateSamplerState(&desc, &samplers[2].p));

	//pointClampSampler
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	HR(Renderer::theDevicePtr->CreateSamplerState(&desc, &samplers[3].p));

	//linearWrapSampler
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	HR(Renderer::theDevicePtr->CreateSamplerState(&desc, &samplers[4].p));

	//LinearClampSampler
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	HR(Renderer::theDevicePtr->CreateSamplerState(&desc, &samplers[5].p));

	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.ComparisonFunc = D3D11_COMPARISON_LESS;
	desc.MinLOD = 0;
	desc.MaxLOD = 0;
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 1;
	desc.BorderColor[2] = 1;
	desc.BorderColor[3] = 1;
	HR(Renderer::theDevicePtr->CreateSamplerState(&desc, &samplers[6].p));

	Renderer::theContextPtr->PSSetSamplers(1, 7, &samplers[0].p);
	Renderer::theContextPtr->HSSetSamplers(1, 7, &samplers[0].p);
	Renderer::theContextPtr->DSSetSamplers(1, 7, &samplers[0].p);
	Renderer::theContextPtr->VSSetSamplers(1, 7, &samplers[0].p);
}

void RenderController::SetPerCameraShaderVariables()
{
	ViewPortManager &vpm = ViewPortManager::GetReference();

	XMVECTOR det;

	cbPerCamera camBuffer;

	camBuffer.gViewProj = *(XMFLOAT4X4 *)&ViewPortManager::GetReference().GetActiveViewProjection();
	camBuffer.gCameraPos = XMFLOAT3(vpm.GetActiveViewPosition().v);
	Float3 camDir = vpm.GetActiveViewForward();
	camDir.normalize();
	camBuffer.gCameraDir = XMFLOAT3(camDir.v);
	
	camBuffer.gFarDist = vpm.GetActiveViewFarClip();
	//camBuffer.gNearDist = vpm.GetActiveViewNearClip();
	float nearDist = vpm.GetActiveViewNearClip();

	camBuffer.gScreenSize.x = (float)Renderer::GetResolutionWidth();
	camBuffer.gScreenSize.y = (float)Renderer::GetResolutionHeight();

	XMStoreFloat4x4(&camBuffer.gInvViewProj, XMMatrixInverse(&det, XMLoadFloat4x4(&camBuffer.gViewProj)));

	camBuffer.gPlaneX = -camBuffer.gFarDist / (camBuffer.gFarDist - nearDist);
	camBuffer.gPlaneY = -camBuffer.gFarDist * nearDist / 
		(camBuffer.gFarDist - nearDist);

	D3D11_MAPPED_SUBRESOURCE edit;
	Renderer::theContextPtr->Map(cameraCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &camBuffer, sizeof(camBuffer));
	Renderer::theContextPtr->Unmap(cameraCBuffer, 0);

	Renderer::theContextPtr->VSSetConstantBuffers(camBuffer.REGISTER_SLOT, 1, &cameraCBuffer.p);
	Renderer::theContextPtr->PSSetConstantBuffers(camBuffer.REGISTER_SLOT, 1, &cameraCBuffer.p);
	Renderer::theContextPtr->GSSetConstantBuffers(camBuffer.REGISTER_SLOT, 1, &cameraCBuffer.p);
	Renderer::theContextPtr->HSSetConstantBuffers(camBuffer.REGISTER_SLOT, 1, &cameraCBuffer.p);
	Renderer::theContextPtr->DSSetConstantBuffers(camBuffer.REGISTER_SLOT, 1, &cameraCBuffer.p);
}

void RenderController::PostRender()
{
	GraphicsProfiler::GetReference().BeginEvent(0, L"Renderin' post-processes started yo!");
	gBufferRT.ActivateSRVs(GBufferStartSlot);

	// Run any post-process effects
	PostProcessController::GetInstance()->Process(sceneTarget);

	gBufferRT.DeactivateSRVs(GBufferStartSlot);

	GraphicsProfiler::GetReference().EndEvent();

	// Solution function for viewing debug buffers
	RenderDebugBuffers();

	Renderer::theContextPtr->OMSetRenderTargets(1, &Renderer::theRenderTargetViewPtr, Renderer::theDepthStencilViewPtr);
	Renderer::ClearDepthAndStencilTarget();

	PostProcessController::GetReference().UpdatePostProcessCBuffer();

	ViewPortManager::GetReference().RenderActiveViewPort();
		
	GraphicsProfiler::GetReference().BeginEvent(0, L"Sprite Batch Begin!");

	spriteBatchPtr->Begin(SpriteSortMode_Deferred, 
		BlendStateManager::GetReference().GetState(BlendStateManager::BS_Alpha));

	RenderSpriteManager::GetReference().Render(spriteBatchPtr);
	TextRenderer::GetReference().Render(spriteFontPtr, spriteBatchPtr);
	PrintConsole::GetReference().Render(spriteFontPtr, spriteBatchPtr);
	spriteBatchPtr->End();
	GraphicsProfiler::GetReference().EndEvent();

	// Sprite batch rendering changes these states
	DepthStencilStateManager::GetReference().ApplyState(DepthStencilStateManager::DSS_Default);
	BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Default);
	//
	Renderer::IncrementRenderCounter();

	// Clear render sets
	ClearRenderSets();
}

void RenderController::RenderDirectionalDepths(EDRendererD3D::DirectionalLightWithShadow *lightPtr,
	unsigned int index)
{
	//assert(0);
	dirLightPtr = lightPtr;
	Renderer::theScreenViewport.Width    = static_cast<float>(theDirectionalShadowSize);
	Renderer::theScreenViewport.Height   = static_cast<float>(theDirectionalShadowSize);
	Renderer::theContextPtr->RSSetViewports(1, &Renderer::theScreenViewport);

	static int test = 1;
	RasterizerStateManager::RasterStates state = RasterizerStateManager::GetReference().GetCurrentState();
	if(1 == test)
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_NOCULL);
	if (2 == test)
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_CCW);
	// Set the shadow map target and apply the light 
	lightPtr->ActivateShadowTarget(index);

	GraphicsProfiler::GetReference().BeginEvent(0, L"Render Directional light shadow casters begin");
	RenderShadowCasters(lightPtr, RenderContext::RS_DEPTH_DIR_LIGHT);
	GraphicsProfiler::GetReference().EndEvent();

	RasterizerStateManager::GetReference().ApplyState(state);

	Renderer::theScreenViewport.Width = static_cast<float>(Renderer::GetResolutionWidth());
	Renderer::theScreenViewport.Height = static_cast<float>(Renderer::GetResolutionHeight());
	Renderer::theContextPtr->RSSetViewports(1, &Renderer::theScreenViewport);
}

void RenderController::RenderPointDepths(PointLightWithShadow *lightPtr)
{
	Renderer::theScreenViewport.Width    = static_cast<float>(thePointShadowSize);
	Renderer::theScreenViewport.Height   = static_cast<float>(thePointShadowSize);
	Renderer::theContextPtr->RSSetViewports(1, &Renderer::theScreenViewport);

	// Set the shadow map target and apply the light 
	lightPtr->ActivateShadowTarget();
	lightPtr->ClearShadowMap();
	GraphicsProfiler::GetReference().BeginEvent(0, L"Render Point light shadow casters begin");
	RenderShadowCasters(lightPtr, 
		static_cast<RenderContext::RenderStage>(RenderContext::RS_DEPTH_PNT_LIGHT));
	GraphicsProfiler::GetReference().EndEvent();

	Renderer::theScreenViewport.Width = static_cast<float>(Renderer::GetResolutionWidth());
	Renderer::theScreenViewport.Height = static_cast<float>(Renderer::GetResolutionHeight());
	Renderer::theContextPtr->RSSetViewports(1, &Renderer::theScreenViewport);
}

void RenderController::RenderSpotDepths(SpotLightWithShadow *lightPtr)
{
	Renderer::theScreenViewport.Width    = static_cast<float>(theSpotShadowSize);
	Renderer::theScreenViewport.Height   = static_cast<float>(theSpotShadowSize);
	Renderer::theContextPtr->RSSetViewports(1, &Renderer::theScreenViewport);

	// Set the shadow map target and apply the light 
	lightPtr->ActivateShadowTarget();

	GraphicsProfiler::GetReference().BeginEvent(0, L"Render Spot light shadow casters begin");
	RenderShadowCasters(lightPtr, RenderContext::RS_DEPTH_SPT_LIGHT);
	GraphicsProfiler::GetReference().EndEvent();

	Renderer::theScreenViewport.Width = static_cast<float>(Renderer::GetResolutionWidth());
	Renderer::theScreenViewport.Height = static_cast<float>(Renderer::GetResolutionHeight());
	Renderer::theContextPtr->RSSetViewports(1, &Renderer::theScreenViewport);
}

void RenderController::RenderCameraDepths()
{
	for(list<RenderContext *>::iterator iter = 
		depthCameraContexts.begin(); iter != depthCameraContexts.end(); ++iter)
	{
		(*iter)->SetRenderStage( RenderContext::RS_DEPTH_CAMERA );
		(*iter)->RenderProcess();
	}
}

void RenderController::RenderShadowCasters(ILight *lightPtr, RenderContext::RenderStage stage)
{
	DepthStencilStateManager::DSStates prevDSState = 
		DepthStencilStateManager::GetReference().GetCurrentState();
	DepthStencilStateManager::GetReference().ApplyState(DepthStencilStateManager::DSS_Default);

	BlendStateManager::BStates prevBSState = BlendStateManager::GetReference().GetCurrentState();
	BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Default);

	RasterizerStateManager::RasterStates prevRSState = 
		RasterizerStateManager::GetReference().GetCurrentState();

	// Render static objects
	for (list<RenderContext * >::iterator iter =
		shadowCasters.begin(); iter != shadowCasters.end(); ++iter)
	{
		(*iter)->SetRenderStage(stage);
		(*iter)->RenderProcess();
	}
	
	DepthStencilStateManager::GetReference().ApplyState(prevDSState);
	BlendStateManager::GetReference().ApplyState(prevBSState);
	RasterizerStateManager::GetReference().ApplyState(prevRSState);
}

void RenderController::InitializeAVisibilityQuery(VisibilityQueryInterface &query)
{
//	query.Initialize(&visibilityTestTarget, m_pDSGBuffers);
}

void RenderController::AddVisiblePointLightShadowed(PointLightSource *light)
{
	visiblePointLightsWithShadow.insert(light);
}

void RenderController::AddVisiblePointLight(PointLightSource *light)
{
	visiblePointLights.insert(light);
}

void RenderController::AddVisibleLight(SpotLightWithShadow *light)
{
	visibleSpotLightsWithShadow.insert(light);
}

void RenderController::AddVisibleLight(SpotLight *light)
{
	visibleSpotLights.insert(light);
}

void RenderController::UpdateMiscShaderData()
{
	D3D11_MAPPED_SUBRESOURCE edit;
	Renderer::theContextPtr->Map(neverChangeCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &miscBuffer, sizeof(miscBuffer));
	Renderer::theContextPtr->Unmap(neverChangeCBuffer, 0);

	Renderer::theContextPtr->VSSetConstantBuffers(miscBuffer.REGISTER_SLOT, 1, &neverChangeCBuffer.p);
	Renderer::theContextPtr->PSSetConstantBuffers(miscBuffer.REGISTER_SLOT, 1, &neverChangeCBuffer.p);
	Renderer::theContextPtr->GSSetConstantBuffers(miscBuffer.REGISTER_SLOT, 1, &neverChangeCBuffer.p);
	Renderer::theContextPtr->HSSetConstantBuffers(miscBuffer.REGISTER_SLOT, 1, &neverChangeCBuffer.p);
	Renderer::theContextPtr->DSSetConstantBuffers(miscBuffer.REGISTER_SLOT, 1, &neverChangeCBuffer.p);
}

void RenderController::ClearRenderSets(void)
{
	// Clear render sets
	list<RenderContext *>::iterator contextIter;

	for( contextIter = shadowCasters.begin(); contextIter != shadowCasters.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	for( contextIter = gBufferContexts.begin(); contextIter != gBufferContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	for( contextIter = depthCameraContexts.begin(); contextIter != depthCameraContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	for( contextIter = unlitOpaqueContexts.begin(); contextIter != unlitOpaqueContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();
	
	for( contextIter = guiContexts.begin(); contextIter != guiContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	if( transparentContextPtr != 0 )
		transparentContextPtr->ClearRenderSet();

	visiblePointLightsWithShadow.clear();
	visibleSpotLightsWithShadow.clear();

	Renderer::IncrementRenderCounter();

}

void RenderController::ClearLightRenderSets(void)
{
		// Clear render sets
	list<RenderContext *>::iterator contextIter;

	for( contextIter = shadowCasters.begin(); contextIter != shadowCasters.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	for( contextIter = gBufferContexts.begin(); contextIter != gBufferContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	for( contextIter = depthCameraContexts.begin(); contextIter != depthCameraContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	visiblePointLightsWithShadow.clear();
	visibleSpotLightsWithShadow.clear();

	Renderer::IncrementRenderCounter();
}

void RenderController::RenderPointLight(PointLightSource *lightSourcePtr)
{
	// Implement a solution for the Deferred Renderer Lab
	RenderPointLightSolution(lightSourcePtr);
}

void RenderController::RenderSpotLight(SpotLightSource *lightSourcePtr)
{
	// Implement a solution for the Deferred Renderer Lab
	RenderSpotLightSolution(lightSourcePtr);
}


void RenderController::RenderDirectionalLight(DirectionalLightSource *lightSourcePtr,
	EDGameCore::Camera *camPtr)
{
	// Fancy cascade solution
	return RenderDirectionalLightSolution(lightSourcePtr, &camPtr->GetShadowFrustumAabb(0), camPtr->GetNumCascades());
	
	const Aabb &aabb = camPtr[0].GetShadowFrustumAabb(2);
	DirectionalLightWithShadow *lightPtr =
		static_cast<DirectionalLightWithShadow *>(lightSourcePtr->GetLightPtr());

	// Simpler single map solution
	return RenderDirectionalLightNoCascadeSolution(lightPtr, aabb);
}

void RenderController::BuildDirViewProjectionCascade(const EDMath::Aabb &aabb,
	EDRendererD3D::DirectionalLightWithShadow *lightPtr, unsigned int index)
{
	BuildDirShadowProjectionSolution(aabb);

	BuildDirShadowViewProjectionSolution(aabb, lightPtr, index);

	BuildDirShadowShimmerFixSolution(index);
}

void RenderController::RenderGBuffers()
{
	// Set camera constant buffer before rendering geometry
	RenderController::GetInstance()->SetPerCameraShaderVariables();

	// Implement a solution for the Deferred Renderer Lab
	RenderGBuffersSolution();

	// Clear our main target before we start rendering lights
	const FLOAT clearColor[4] = { 0, 0, 0, 1 };
	sceneTarget.ActivateTarget();
	sceneTarget.ClearRenderTargetView(clearColor);
	sceneTarget.ClearDepthStencilView(D3D11_CLEAR_STENCIL);
}

void RenderController::RenderUnlitObjects()
{
	RenderUnlitObjectsSolution();
}

void RenderController::RenderTransparentObjects()
{
	RenderTransparentObjectsSolution();
}

void RenderController::RenderDebugPrimitives()
{
	debugLines.GetContent()->RenderProcess();
	debugTriangles.GetContent()->RenderProcess();
}
