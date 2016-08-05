#pragma once

#include "RenderContext.h"
#include "RenderTexture.h"
#include "RenderMaterial.h"



namespace EDRendererD3D
{
	/// A specialization of RenderContext which includes the texture to be used.
	/// Storing the texture in the RenderContext results in fewer texture context switches.
	/// This will also result in more unique contexts being needed, so more effect context
	/// switches will occur but his is still an optimization under most applications.
    class RenderContextWithTexture : public RenderContext
    {
    private:
		/// The diffuse textures to be used by this context.
		//std::vector<RenderTexture *> m_Textures;

        RenderMaterial *renderMaterialPtr;

		static void ContextSharedRenderFunc(RenderNode &node);

    public:
		/// Adds a texture to be used by this context.
		//void AddTexture(RenderTexture *texture);
        //void ClearTextures() { m_Textures.clear(); }

		
//        inline std::vector<RenderTexture *> &GetTextures() { return  m_Textures; }
		/// A RenderFunc to be used by RenderContextWithTexture instances.
		/// Sets render states controlled by this context, including diffuse textures
		/// and renders the contained RenderSet.
        /// Sets the stream to the buffer containing positions, normals and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPosNormalUVRenderFunc(RenderNode &node);

        /// A RenderFunc to be used by RenderContextWithTexture instances.
		/// Sets render states controlled by this context, including diffuse textures
		/// and renders the contained RenderSet.
        /// Sets the stream to the buffer containing positions, normals tangents and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPosNormalTangentUVRenderFunc(RenderNode &node);

		/// A RenderFunc to be used by RenderContextWithTexture instances.
		/// Sets render states controlled by this context, including diffuse textures
		/// and renders the contained RenderSet.
        /// Sets the stream to the buffer containing only positions
		/// \param node - A RenderContext reference to render.
		static void ContextPositionRenderFunc(RenderNode &node);

		/// A RenderFunc to be used by RenderContextWithTexture instances.
		/// Sets render states controlled by this context, including diffuse textures
		/// and renders the contained RenderSet.
        /// Sets the stream to the buffer containing positions and UVs
		/// \param node - A RenderContext reference to render.
		static void ContextPositionUVRenderFunc(RenderNode &node);

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

        static void ContextWithTextureSharedRenderFuncSolution(RenderNode &node);
        static void ContextWithTexturePosNormalUVRenderFuncSolution(RenderNode &node);
	    static void ContextWithTexturePositionRenderFuncSolution(RenderNode &node);
        static void ContextWithTexturePositionUVRenderFuncSolution(RenderNode &node);
		static void ContextPosBoneWeightNormalUVRenderFuncSolution(RenderNode &node);
		static void ContextPosBoneWeightNormalTanUVRenderFuncSolution(RenderNode &node);
    };
}