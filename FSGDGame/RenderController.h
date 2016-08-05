#pragma once 

#include "../DirectXTK/Inc/SpriteFont.h"
#include "../EDRendererD3D/RenderContextWithTexture.h"
#include "../EDRendererD3D/DirectionalLightWithShadow.h"
#include "../EDRendererD3D/PointLightWithShadow.h"
#include "../EDRendererD3D/SpotLightWithShadow.h"
#include "../EDRendererD3D/SpotLight.h"
#include "../EDRendererD3D/RenderMesh.h"
#include "../EDRendererD3D/RenderShapeWithTexture.h"
#include "../EDRendererD3D/RenderShapeTarget.h"
#include "../EDRendererD3D/RenderTarget.h"
#include "../EDRendererD3D/CubeRenderTarget.h"
#include "../EDRendererD3D/VisibilityQueryInterface.h"
#include "../EDRendererD3D/RenderString.h"
#include "../EDRendererD3D/RenderSprite.h"
#include "../EDUtilities/ContentManager.h"
#include "../EDMath/float4x4.h"
#include "../EDGameCore/Camera.h"

#include "ForwardLightManager.h"

class PointLightSource;
class SpotLightSource;
class DirectionalLightSource;

/// Container class for non-data driven rendering components.
class RenderController
{
private:
	/// Sprite batch handler
	DirectX::SpriteBatch *spriteBatchPtr;
	/// Sprite font handler used with sprite batches for on screen text rendering
	DirectX::SpriteFont *spriteFontPtr;

	/// RenderContext to be used for rendering the 2D objects.
	/// This context is added to the RenderContext Manager with the name 2DContext.
	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> twoDContext;
	/// A render context used to render directional lights in a deferred renderer.
	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> directionDeferredContextHandle;
	/// A render context used to render point lights in a deferred renderer.
	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> pointLightWithShadowsContextHandle;
	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> pointLightContextHandle;
	/// A render context used to render spot lights in a deferred renderer.
	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> spotLightWithShadowsContextHandle;
	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> spotLightContextHandle;
	/// Render context used for debug line rendering
	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> debugLines;
	/// Render context used for debug triangle rendering
	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> debugTriangles;
	/// Render Context used for rendering standard transparent objects
	EDRendererD3D::RenderContext* transparentContextPtr;

	/// A collection of contexts that hold data that should cast shadows.
	std::list<EDRendererD3D::RenderContext *> shadowCasters;
	/// A collection of contexts that hold data to be rendered to basic G-Buffer
	std::list<EDRendererD3D::RenderContext *> gBufferContexts;
	/// A collection of contexts that hold data to be rendered to depth target
	std::list<EDRendererD3D::RenderContext *> depthCameraContexts;
	/// A collection of contexts that hold data to be rendered without lighting 
	/// during the forward rendering pass
	std::list<EDRendererD3D::RenderContext *> unlitOpaqueContexts;
	/// A collection of contexts used to render GUI elements
	std::list<EDRendererD3D::RenderContext *> guiContexts;

	/// Constant Buffer containing data that changes with the camera(s)
	CComPtr<ID3D11Buffer> cameraCBuffer;
	/// Constant Buffer containing data that rarely or never changes
	CComPtr<ID3D11Buffer> neverChangeCBuffer;

	///////////////////////////////////////////////////////////////////////
	// Directional light data - consider changing this to more closely 
	// macth point and spot light solutions which allow for multiple 
	// instances and are data driven

	/// The concatenated view projection of the directional light, 
	/// needed for shadow mapping
	static const int NumCascades = 3;
	DirectX::XMFLOAT4X4 dirViewProjection[NumCascades];
	/// The projection matrix to be used to represent the light space of the 
	/// directional light.
	EDMath::Float4x4 lightProjection;
	EDMath::Float4x4 lightView;
	EDRendererD3D::DirectionalLightWithShadow *dirLightPtr;

	/// The collection of point lights currently in view
	std::set<PointLightSource *> visiblePointLights;
	//std::set<EDRendererD3D::PointLight *> visiblePointLights;
	std::set<PointLightSource *> visiblePointLightsWithShadow;
	//std::set<EDRendererD3D::PointLightWithShadow *> visiblePointLightsWithShadow;
	/// The collection of spot lights currently in view
	std::set<EDRendererD3D::SpotLightWithShadow *> visibleSpotLightsWithShadow;
	std::set<EDRendererD3D::SpotLight *> visibleSpotLights;

	/// The Render Target to be used during GBuffer render phase
	EDRendererD3D::RenderTarget gBufferRT;
	// The starting slot for GBuffer SRVs
	const static int GBufferStartSlot = 9;
	/// The Render Target to be presented to the screen each frame
	EDRendererD3D::RenderTarget sceneTarget;
	/// The Render Target to be used for rendering transparent objects
	/// This target will typically have the main view as well as some
	/// G-Buffer views for rendering transparent G-Buffer data
	EDRendererD3D::RenderTarget transSceneTarget;

	/// The view to render diffuse colors to during GBuffer phase
	EDRendererD3D::RenderTargetSurface gBufferDiffuseTargetView;
	/// The view to render normals to during GBuffer phase
	EDRendererD3D::RenderTargetSurface gBufferNormalsTargetView;
	/// The view to render specular colors to during GBuffer phase
	EDRendererD3D::RenderTargetSurface gBufferSpecularTargetView;
	/// The view to render depths to during GBuffer phase
	EDRendererD3D::RenderTargetSurface gBufferDepthTargetView;
	/// The view to render to later be presented to the screen
	EDRendererD3D::RenderTargetSurface sceneTargetView;

	/// The render target for rendering all spot light shadow depths
	EDRendererD3D::RenderTarget spotDepths;
	/// The view for rendering all spot light shadow depths
	EDRendererD3D::RenderTargetSurface spotDepthsTargetView;

	/// RenderTarget for Occlusion/Visibility Queries
	EDRendererD3D::RenderTarget visibilityTestTarget;

	/// A quad mesh that encomposes the screen, in clip space.
	EDRendererD3D::RenderMesh *screenQuadMeshPtr;
	/// A quad mesh that encomposes one quarter of the screen, in clip space.
	EDRendererD3D::RenderMesh quarterScreenQuadMesh;
	/// A shape used to define a full screen render target.
	EDRendererD3D::RenderShapeTarget screenQuad;
	/// A shape used to render debug views, such as the GBuffer results, in real time
	EDRendererD3D::RenderShapeTarget debugBufferShape;
	/// Should a debug view be rendered
	bool shouldRenderDebugBuffer;
	/// Which debug view should be rendered
	unsigned int debugBufferToRender;

	/// Constant Buffer data structure object
	cbMisc miscBuffer;

	/// A boolean used to check if the Resize method of this class is being called for the 
	/// first time as certain code should only be run under that circumstance, this is a bit 
	/// of a hack and should be refactored
	bool firstResize;

	ForwardLightManager forwardLightManager;

	/// A helper method used to initialize RenderContexts.
	/// This should eventually be data driven.
	void CreateContexts();
	/// The method used to create the Render Targets and views rendering
	void CreateRenderTargets();
	/// The method used to create the Render Targets and view used as Shadow Maps
	void CreateDepthBuffers();
	/// The method used to create interfaces to shader buffers
	void CreateConstantBuffers();
	/// A helper method used to organize rendering camera depth values.
	void RenderCameraDepths();
	/// A helper method used to organize rendering shadow depth values.
	void RenderShadowCasters(EDRendererD3D::ILight *lightPtr, 
		EDRendererD3D::RenderContext::RenderStage stage);
	/// Renders shadow depths for the spot light.
	void RenderSpotDepths(EDRendererD3D::SpotLightWithShadow *lightPtr);
	/// Renders shadow depths for the point lights.
	void RenderPointDepths(EDRendererD3D::PointLightWithShadow *lightPtr);
	/// Renders shadow depths for the directional light.
	void RenderDirectionalDepths(EDRendererD3D::DirectionalLightWithShadow *lightPtr, unsigned int index);

	/// A orginizational method to handle console command creation
	void InitializeConsoleCommands();
	/// The method used to create texture samplers
	void InitializeTextureSamplers();
	/// Initializes data used for rendering debug buffers
	void InitializeDebugBuffers();
	/// Method used to render debug buffers
	void RenderDebugBuffers();
	/// Creates the RenderTarget to presented to the screen and initializes
	/// the ViewPortManager system
	void CreateViewPorts(UINT viewPorts);

	/// Lab solution method, ignore this
	void RenderGBuffersSolution();
	/// Lab solution method, ignore this
	void RenderLightsAndForwardPassSolution();

	/// Lab solution method, ignore this
	void RenderPointLightSolution(PointLightSource *lightSourcePtr);
	/// Lab solution method, ignore this
	void RenderSpotLightSolution(SpotLightSource *lightSourcePtr);
	/// Lab solution method, ignore this
	void RenderDirectionalLightSolution(DirectionalLightSource *lightSourcePtr,
		const EDMath::Aabb *cascadeAabbs, unsigned int numCascades);
	void RenderDirectionalLightNoCascadeSolution(
		EDRendererD3D::DirectionalLightWithShadow *lightPtr, const EDMath::Aabb &aabb);

	/// Lab solution method, ignore this
	void RenderUnlitObjectsSolution();
	/// Lab solution method, ignore this
	void RenderTransparentObjectsSolution();

	/// Lab solution method, ignore this
	void BuildDirShadowProjectionSolution(const EDMath::Aabb& aabb);
	/// Lab solution method, ignore this
	void BuildDirShadowViewProjectionSolution(const EDMath::Aabb& aabb, 
		EDRendererD3D::DirectionalLightWithShadow *lightPtr, unsigned int index);
	/// Lab solution method, ignore this
	void BuildDirShadowShimmerFixSolution(unsigned int index);

	/// Default constructor, made private for singleton architecture
	RenderController();
	/// Copy constructor, made private for singleton architecture
	RenderController(const RenderController&){}
	/// Assignment operator, made private for singleton architecture
	RenderController& operator=(const RenderController&){ return *this; }
	/// The one and only instance of the class
	static RenderController* instancePtr;

	/// Called each time the view changes to build the final view projection
	/// matrix used for the directional light shadow maps.
	// void BuildDirViewProjection(const EDCollision::FrustumHull &frustumHull);
	void BuildDirViewProjectionCascade(const EDMath::Aabb& aabb,
		EDRendererD3D::DirectionalLightWithShadow *lightPtr, unsigned int index);
	/// Method to create the initial view and projection matrices to be used 
	/// for the directional light. This method only needs to be called when 
	/// the frustum changes size.
	// void BuildInitialiDirMatrices(const EDCollision::FrustumHull &frustumHull);
	//void BuildInitialiDirMatrices(const EDMath::Aabb& aabb);
public:		
	/// Accessor to a pointer to the instance of this class
	static RenderController *GetInstance();
	/// Destroys the instance of this class
	static void DeleteInstance(void);

	/// The resolutions for shadow maps - change spot light to use directional target?  
	/// Better effect, less total memory, slower render...
	static int thePointShadowSize;
	static int theDirectionalShadowSize;
	static int theSpotShadowSize;

	/// Enumeration of the debug views available to be rendered
	enum GBufferEnum{ GB_Depth, GB_Diffuse, GB_Normal, GB_Specular, GB_DirShadow0, GB_DirShadow1, GB_DirShadow2};
	/// Destructor
	~RenderController();
	/// Initializes the contexts, textures and lights
	/// \param viewPorts - The number of view ports to use
	void Initialize();

	/// Handles additional rendering needs after standard deferred shading,
	/// this includes post-process effects, sprites, debug rendering and clean up
	void PostRender();
	/// Renders the G-Buffer values to a texture target.
	void RenderGBuffers();

	void RenderDebugPrimitives();
	/// The method used to update the per-camera constant buffers
	void SetPerCameraShaderVariables();

	void ClearRenderSets(void);
	void ClearLightRenderSets(void);
	/// Method used to add RenderContexts to be rendered, puts the contexts in the appropriate
	/// list based on state.
	void AddRenderContext(EDRendererD3D::RenderContext* pContext);

	/// Initializes a VisibilityQueryInterface instance, using a single RenderTarget
	/// for all queries and the depth buffer used for general rendering
	/// \param query - The VisibilityQueryInterface to initialize
	void InitializeAVisibilityQuery(EDRendererD3D::VisibilityQueryInterface &query);

	/// Returns a mesh representing the screen
	inline EDRendererD3D::RenderMesh *GetScreenQuadPtr() { return screenQuadMeshPtr; }
	/// Returns the context used to render main scene data
	inline EDRendererD3D::RenderContext *GetScreenContextPtr() { return twoDContext.GetContent(); }
	/// Returns the shape used for rendering screen data
	inline EDRendererD3D::RenderShapeTarget *GetScreenShapePtr() { return &screenQuad; }

	inline cbMisc &GetMiscShaderData() { return miscBuffer; }

	/// Applies any changes made to the miscellaneous shader constant buffer
	void UpdateMiscShaderData();

	/// Method used to add a point light to the visible set this frame
	void AddVisiblePointLightShadowed(PointLightSource *light);
	void AddVisiblePointLight(PointLightSource *light);
	/// Method used to add a spot light to the visible set this frame
	void AddVisibleLight(EDRendererD3D::SpotLightWithShadow *light);
	void AddVisibleLight(EDRendererD3D::SpotLight *light);

	/// Accessor method to retrieve the size of the point light shadow map
	static inline int GetPointShadowSize(void){ return thePointShadowSize; }

	/// Accessor method used to retrieve the RenderTarget used for spot light shadow maps
	EDRendererD3D::RenderTarget &GetSpotShadowRenderTarget() { return spotDepths; }

	/// Toggles whether debug buffers should be rendered
	inline void ToggleDebugRender() { shouldRenderDebugBuffer = !shouldRenderDebugBuffer; }
	/// Sets which debug buffer should be rendered
	inline void SetDebugBufferToRender(unsigned int index) { debugBufferToRender = index; }

	/// Method used to initialize or adjust members requiring knowledge of the screen size
	void UpdateResolution();
	/// Method used to (re)initialize post-process effects
	void CreatePostProcessEffects();
	//////////////////////////////////////////////////////////////////////////
	/// These methods are for console command handling and are defined 
	/// in RenderControllerCommands.cpp
	void SetShadowDepthOffset(float val);
	void SetDisplacementScale(float val);

	void RenderPointLight(PointLightSource *lightPtr);
	void RenderSpotLight(SpotLightSource *lightPtr);
	//void RenderDirectionalLight(DirectionalLightSource *lightPtr, const EDMath::Aabb &aabb);
	void RenderDirectionalLight(DirectionalLightSource *lightPtr, EDGameCore::Camera *camPtr);
		//const EDMath::Aabb &cameraAabb, const EDMath::Aabb cascadeAabbs[3]);
	void RenderUnlitObjects();
	void RenderTransparentObjects();

	DirectX::XMFLOAT4X4 &GetDirViewProjection(unsigned int index) { return dirViewProjection[index]; }
	DirectX::XMFLOAT4X4 *GetDirViewProjection() { return dirViewProjection; }

	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> &GetPointLightWithShadowsContextHandle()
	{
		return pointLightWithShadowsContextHandle;
	}

	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> &GetSpotLightWithShadowsContextHandle()
	{
		return spotLightWithShadowsContextHandle;
	}

	EDUtilities::ContentHandle<EDRendererD3D::RenderContext> &GetDirectionalLightWithShadowsContextHandle()
	{
		return directionDeferredContextHandle;
	}

	ForwardLightManager &GetForardLightManager(void) { return forwardLightManager; }
};
