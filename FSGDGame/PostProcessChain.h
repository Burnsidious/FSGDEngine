#pragma once
#include "../EDRendererD3D/PostProcess.h"
#include "../EDRendererD3D/RenderTarget.h"
#include "../EDRendererD3D/ConstantBuffers.h"


class PostProcessChain
{
public:
	enum PostProcessTransistion { PPT_NoChange = 0, PPT_DownSample, PPT_UpSample };
private:

	struct PostProcessEffect
	{
		EDRendererD3D::PostProcess *postProcessPtr;
		PostProcessTransistion transition;

		PostProcessEffect() : postProcessPtr(nullptr), transition(PPT_NoChange)	{}
		PostProcessEffect(EDRendererD3D::PostProcess *_postProcessPtr,	
			PostProcessTransistion _transition) :
			postProcessPtr(_postProcessPtr), transition(_transition){}

		friend bool operator==(const PostProcessEffect &lhs, const PostProcessEffect &rhs)
		{
			return lhs.postProcessPtr == rhs.postProcessPtr && lhs.transition == rhs.transition;
		}
	};

	/// The list of post-process effects to be run.
	std::list< PostProcessEffect > processChain;

	EDRendererD3D::RenderTarget renderTarget1;
	EDRendererD3D::RenderTargetSurface renderTargetView1;

	EDRendererD3D::RenderTarget renderTarget2;
	EDRendererD3D::RenderTargetSurface renderTargetView2;

	cbPostProcess &postShaderData;
	CComPtr<ID3D11Buffer> postShaderDataCBuffer;

	PostProcessTransistion sampleState;

	/// Pointer to the RenderTarget post-process effects should use as the 
	/// source data.
	EDRendererD3D::RenderTarget *sourceTargetPtr;
	
	/// Pointer to the RenderTarget to be used as the destination for 
	/// post-process effects.
	EDRendererD3D::RenderTarget *destinationTargetPtr;

	static const int PRE_POST_MAP_REGISTER = 7;

	void SwapTargets();
	void UpdateShaderData();
	void UpdateViewports(std::list< PostProcessEffect >::iterator &processIter);

	void InitializeSolution();
	void RunSolution(EDRendererD3D::RenderTarget &mainTarget);
	void SwapTargetsSolution();
	void UpdateShaderDataSolution();
	void UpdateViewportsSolution(std::list< PostProcessEffect >::iterator &processIter);

public:

	PostProcessChain(cbPostProcess &_postShaderData);
	~PostProcessChain();
	/// Initialize the post-process effect swap chain.
	void Initialize();

	/// Run all post-process effects contained.
	/// \param mainTarget - The RenderTarget containing the original source
	/// material to be used in post.  This target will have the result of the
	/// post-process chain when this method returns.
	void Run(EDRendererD3D::RenderTarget &mainTarget);

	/// Adds a post-process effect to the swap chain.
	/// \param _postProcessPtr - A pointer to an existing PostProcess to be added for processing
	/// \param _transition - Controls whther a resolution transition should occur when this effect is processed
	/// \return Returns a pointer to the created PostProcessEffect, this should only be needed for later removing the eefect
	inline PostProcessEffect *AddPostProcess(EDRendererD3D::PostProcess *_postProcessPtr, 
		PostProcessTransistion _transition = PPT_NoChange)
	{
		processChain.push_back(PostProcessEffect(_postProcessPtr, _transition));
		return &(*prev(processChain.end(), 1));
	}

	/// Removes a post-process effect from the swap chain.
	/// \param effectToRemovePtr - A pointer to the effect to be removed.
	inline size_t RemovePostProcess(PostProcessEffect *effectToRemovePtr)
	{
		processChain.remove(*effectToRemovePtr);
		//processChain.erase(next(processChain.begin(), ID));
		return processChain.size();
	}

	size_t RemovePostProcess(EDRendererD3D::PostProcess *effectToRemovePtr);

	/// Clears the list of PostProcess pointer's contained
	inline void Clear()
	{
		processChain.clear(); 
	}
};