#pragma once

#include "../EDRendererD3D/PostProcess.h"
#include "PostProcessChain.h"

class PostProcessController
{
public:
	~PostProcessController(void);

	static PostProcessController *GetInstance();
	static PostProcessController &GetReference();
	static void DeleteInstance();

	// Initializes all contained post process instances using the provided mesh for rendering
	void InitializePostEffects(EDRendererD3D::RenderMesh *screenQuadMeshPtr);
	// Initializes PostProcessSwapChain instances with current screen resolution data
	void UpdateResolution(void);
	// Clears all post process effects from all chains
	void Clear(void);

	void Process(EDRendererD3D::RenderTarget &renderTarget);

	void PushBackPostProcess(EDRendererD3D::PostProcess *process,	
		PostProcessChain::PostProcessTransistion _transition = PostProcessChain::PPT_NoChange);

	void RemovePostProcess(EDRendererD3D::PostProcess *process);

	inline cbPostProcess &GetPostShaderData() { return postShaderData; }

	void UpdatePostProcessCBuffer(void);
	//////////////////////////////////////////////////////////////////////////
	/// These methods are for console command handling and are defined 
	/// in RenderControllerCommands.cpp
	void AddDepthOfFieldPostProcess();
	void AddDrawEdgesPostProcess();
	void AddSobelEdgesPostProcess();
	void AddSobelNormEdgesPostProcess();
	void AddGaussianBlurHPostProcess();
	void AddGaussianBlurVPostProcess();
	void AddDownSamplePostProcess();
	void AddUpSamplePostProcess();
	void AddBloomHPostProcess();
	void AddBloomVPostProcess();
	void AddBrightPassPostProcess();
	void AddCombinePostProcess();
	void AddBloomPostProcess();
	void AddFXAAPostProcess();
	void AddChainPostProcess();
	void AddPostTestProcess();
	void AddFogProcess();
    void SetDepthOffset(float val);
    void SetEdgeIntensity(float val);
    void SetEdgeWidth(float val);
	void SetFocusDistance(float val);
	void SetFocusRange(float val);
	//////////////////////////////////////////////////////////////////////////

private:

	EDRendererD3D::PostProcess edgePostProcess;
	EDRendererD3D::PostProcess edgeSobelPostProcess;
	EDRendererD3D::PostProcess edgeSobelNormPostProcess;
	EDRendererD3D::PostProcess verticalBlurPostProcess;
	EDRendererD3D::PostProcess horizontalBlurPostProcess;
	EDRendererD3D::PostProcess depthOfFieldPostProcess;
	EDRendererD3D::PostProcess brightPostProcess;
	EDRendererD3D::PostProcess verticalBloomPostProcess;
	EDRendererD3D::PostProcess horizontalBloomPostProcess;
	EDRendererD3D::PostProcess combinePostProcess;
	EDRendererD3D::PostProcess postTestProcess;
	EDRendererD3D::PostProcess fxaaPostProcess;
	EDRendererD3D::PostProcess passThroughPostProcess;
	EDRendererD3D::PostProcess fogPostProcess;

	/// The list of post process chains that should be applied to the scene this frame
	std::list<PostProcessChain *> postProcessChains;

	/// Constant Buffer data structure object
	cbPostProcess postShaderData;
	CComPtr<ID3D11Buffer> postShaderDataCBuffer;

	// Singelton
	static PostProcessController *instancePtr;
	PostProcessController(void );

	PostProcessController(const PostProcessController &) {}
	PostProcessController(const PostProcessController &&) {}
	PostProcessController &operator=(const PostProcessController &) { return * this; }
	PostProcessController &operator=(const PostProcessController &&) { return * this; }
};

