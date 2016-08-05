#pragma once

#include "RenderNode.h"
#include "RenderSetSorted.h"
#include "Light.h"
#include "VertexFormats.h"
#include "ShaderEffect.h"
#include "RenderMaterial.h"
#include "Renderer.h"

#include "../EDUtilities/ContentManager.h"

namespace EDRendererD3D
{
	/// An object to define a collection of render states.
	/// The RenderContext class is a specialization of RenderNode that is used to set
	/// any render states that can be shared.  For the fastest rendering possible
	/// RenderNode batches should be sorted in a manner to have as few RenderContexts as possible
	class RenderContext : public RenderNode
	{
	public:
		/// Enumerated stages of rendering that a context might have individual techniques for.
		enum RenderStage{	RS_DEPTH_SPT_LIGHT = 0, RS_DEPTH_PNT_LIGHT, RS_DEPTH_DIR_LIGHT, RS_DEPTH_CAMERA, RS_GBUFFER, 
			RS_SPT_LIGHT, RS_PNT_LIGHT, RS_DIR_LIGHT, RS_UNLIT, RS_TRANSPARENT, 
			RS_GUI, RS_MAX };

		/// Assigns pointers to null and technique name to Basic
		RenderContext();
		/// Cleans up D3D components and dynamic memory.
		~RenderContext();

		/// Creates the effect to be used by this context.
		/// \param effectFile - a character string defining the name of the file containing the effect
		/// \return Returns true if the build was successful
		bool CreateEffect(const char *effectFile);

		/// A helper method used to retrieve and store shader variable handles.
		void InitializeHandles();

		////////////////////////////////////////////////////////////////////////////
		// RenderFuncs
		////////////////////////////////////////////////////////////////////////////
		/// A RenderFunc to be used by RenderContext instances.
		/// Sets render states controlled by this context and renders the contained RenderSet.
		/// Sets the stream to the buffer containing positions, normals and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPosNormalUVRenderFunc(RenderNode &node);
		static void ContextTransparentPosNormalUVRenderFunc(RenderNode &node);
		static void ContextPosNormalUVNoCullRenderFunc(RenderNode &node);
		
		/// A RenderFunc to be used by RenderContext instances.
		/// Sets render states controlled by this context and renders the contained RenderSet.
		/// Sets the stream to the buffer containing positions, normals and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPosNormalTangentUVRenderFunc(RenderNode &node);
		static void ContextTransparentPosNormalTangentUVRenderFunc(RenderNode &node);
		static void ContextPosNormalTangentUVNoCullRenderFunc(RenderNode &node);
		
		/// A RenderFunc to be used by RenderContext instances.
		/// Sets render states controlled by this context and renders the contained RenderSet.
		/// Sets the stream to the buffer containing only positions
		/// \param node - A RenderContext reference to render.
		static void ContextPositionRenderFunc(RenderNode &node);
		static void ContextTransparentPositionRenderFunc(RenderNode &node);

		/// A RenderFunc to be used by RenderContext instances.
		/// Sets render states controlled by this context and renders the contained RenderSet.
		/// Sets the stream to the buffer containing positions and colors
		/// \param node - A RenderContext reference to render.
		static void ContextPositionColorRenderFunc(RenderNode &node);
		static void ContextTransparentPositionColorRenderFunc(RenderNode &node);

		/// A RenderFunc to be used by RenderContext instances.
		/// Sets render states controlled by this context and renders the contained RenderSet.
		/// Sets the stream to the buffer containing positions and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPositionUVRenderFunc(RenderNode &node);
		static void ContextTransparentPositionUVRenderFunc(RenderNode &node);

		/// A RenderFunc to be used by RenderContextWithTexture instances.
		/// Sets render states controlled by this context, including diffuse textures
		/// and renders the contained RenderSet.
        /// Sets the stream to the buffer containing positions, bone indices, skin weights, 
		/// normals and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPosBoneWeightNormalUVRenderFunc(RenderNode &node);
		
		/// A RenderFunc to be used by RenderContextWithTexture instances.
		/// Sets render states controlled by this context, including diffuse textures
		/// and renders the contained RenderSet.
        /// Sets the stream to the buffer containing positions, bone indices, skin weights, 
		/// normals, tangents and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPosBoneWeightNormalTanUVRenderFunc(RenderNode &node);

		/// Loads a RenderContext from a specified XML file.
		/// \param xmlFileNamePtr - File path to the RenderContext to load from XML format.
		/// \param indentifier - A unique identifier, unused in this context. Exists only to match function prototype.
		static RenderContext* LoadXML(const char* xmlFileNamePtr, const char* indentifier = 0);

		////////////////////////////////////////////////////////////////////////////
		// Solution declarations
		////////////////////////////////////////////////////////////////////////////
		static void ContextSharedRenderFuncSolution(RenderNode &node);
		static void ContextPosNormalUVRenderFuncSolution(RenderNode &node);
		static void ContextPosNormalTangentUVRenderFuncSolution(RenderNode &node);
		static void ContextPositionRenderFuncSolution(RenderNode &node);
		static void ContextPositionUVRenderFuncSolution(RenderNode &node);
		static void ContextPositionColorRenderFuncSolution(RenderNode &node);
		static void ContextPosBoneWeightNormalUVRenderFuncSolution(RenderNode &node);
		static void ContextPosBoneWeightNormalTanUVRenderFuncSolution(RenderNode &node);
		////////////////////////////////////////////////////////////////////////////
		// Inlines
		////////////////////////////////////////////////////////////////////////////

		/// \return Returns the name of the technique for the currently set rendering stage. Returns NULL if none is set.
		inline const ShaderTechnique *GetRenderStageTechnique(void)
		{ 
			return  renderStageTechniques[currentStage]; 
		}

		/// \return Returns the name of the technique for a specific rendering stage. Returns NULL if none is set.
		inline const ShaderTechnique *GetRenderStageTechnique(RenderStage stage)
		{ 
			return renderStageTechniques[stage];
		}

		/// \return Returns the vertex format of this RenderContext
		inline VertexFormat GetVertexFormat(void){ return vertexFormat; }

		/// Sets the technique name to use for a specific stage of rendering.
		/// \param stage - The rendering stage to set the technique name for.
		/// \param pTechniqueName - The name to assign to the rendering stage specified by stage.
		inline void SetRenderStageTechnique(RenderStage stage, ShaderTechnique *techniquePtr)
		{
			renderStageTechniques[stage] = techniquePtr; 
		}

		/// Sets the current rendering stage of this RenderContext to the specified stage.
		/// \param stage - The rendering stage to set the current stage to.
		inline void SetRenderStage(RenderStage stage){ currentStage = stage; }

		/// Creates the RenderSet to be used by this context.
		/// \param bSorted  - If true creates a render set with sorting
		/// \param SortFunc - A function pointer to the method to be used for sorting.
		///                    This parameter is ignored if bSorted is false.
		inline void CreateRenderSet(bool bSorted = false, 
			bool (*SortFunc)(RenderNode *left, RenderNode *right) = 0)
		{
			if(bSorted)
			{
				renderSetPtr = new RenderSetSorted;
				((RenderSetSorted *)renderSetPtr)->SortFunc = SortFunc;
			}
			else
				renderSetPtr = new RenderSet;
		}
		/// \return Returns a reference to the RenderSet being used by this context.
		inline RenderSet &GetRenderSet(void) { return *renderSetPtr; }
		/// Adds a RenderNode to the RenderSet of this context.  This should be called
		/// on all RenderNodes that should be rendered with this context each frame.
		/// \param nodePtr - The RenderNode to be rendered in this batch.
		inline void AddRenderNode(RenderNode *nodePtr)
		{
			renderSetPtr->AddRenderNode(nodePtr);
		}
		/// Clears the RenderSet of this context.
		inline void ClearRenderSet(void) { renderSetPtr->ClearRenderSet();}

		inline void EffectSetDiffuseMap(ID3D11ShaderResourceView * texture) 
		{ 
			Renderer::theContextPtr->PSSetShaderResources(RTT_Diffuse, 1, &texture);
			Renderer::theContextPtr->DSSetShaderResources(RTT_Diffuse, 1, &texture);
		}
		inline void EffectSetAmbientMap(ID3D11ShaderResourceView * texture) 
		{ 
			Renderer::theContextPtr->PSSetShaderResources(RTT_Ambient, 1, &texture);
			Renderer::theContextPtr->DSSetShaderResources(RTT_Ambient, 1, &texture);
		}
		inline void EffectSetSpecularMap(ID3D11ShaderResourceView * texture) 
		{ 
			Renderer::theContextPtr->PSSetShaderResources(RTT_Specular, 1, &texture);
			Renderer::theContextPtr->DSSetShaderResources(RTT_Specular, 1, &texture);
		}
		inline void EffectSetEmissiveMap(ID3D11ShaderResourceView * texture) 
		{ 
			Renderer::theContextPtr->PSSetShaderResources(RTT_Emmisive, 1, &texture);
			Renderer::theContextPtr->DSSetShaderResources(RTT_Emmisive, 1, &texture);
		}
		inline void EffectSetNormalMap(ID3D11ShaderResourceView * texture) 
		{ 
			Renderer::theContextPtr->PSSetShaderResources(RTT_Normal, 1, &texture);
			Renderer::theContextPtr->DSSetShaderResources(RTT_Normal, 1, &texture);
		}
		inline void EffectSetReflectedMap(ID3D11ShaderResourceView * texture) 
		{ 
			Renderer::theContextPtr->PSSetShaderResources(RTT_Reflected, 1, &texture);
			Renderer::theContextPtr->DSSetShaderResources(RTT_Reflected, 1, &texture);
		}
		inline void EffectSetPerObjectTextures(ID3D11ShaderResourceView *textures[RTT_Count]) 
		{ 
			Renderer::theContextPtr->PSSetShaderResources(RTT_Ambient, RTT_Count, textures);
			Renderer::theContextPtr->DSSetShaderResources(RTT_Ambient, RTT_Count, textures);
		}

		inline EDUtilities::ContentHandle<ShaderEffect> &GetEffect() { return effectPtr; }

		inline bool CastsShadows(void)const{ return castShadows; }
	protected:
		/// The shader effect used by this RenderContext
		EDUtilities::ContentHandle<ShaderEffect> effectPtr;
		/// The techniques for each stage of rendering for this context
		ShaderTechnique *renderStageTechniques[RS_MAX];

		/// The linked list of RenderNodes to be rendered using this context.
		/// This is typically filled just RenderShape instances.
		RenderSet *renderSetPtr;

		/// A RenderFunc to be used by other RenderContext RenderFuncs for shared functionality.
		/// Sets render states controlled by this context and renders the contained RenderSet.
		/// \param node - A RenderContext reference to render.
		static void ContextSharedRenderFunc(RenderNode &node);

		void SetRenderStageTechnique(TiXmlElement *elementPtr,
			const char *techName, const char *xmlFileNamePtr, RenderStage stage);

		/// The vertex format used by this rendercontext
		VertexFormat vertexFormat;

		/// The currently set render stage for this context, used as an index into renderStageTechniques
		RenderStage currentStage;

		bool castShadows;

	};
}
