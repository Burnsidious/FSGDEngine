#pragma once

#include "RenderShapeWithTexture.h"
#include "RenderMaterial.h"
#include "RenderForm.h"
#include "../EDUtilities/ContentManager.h"

namespace EDRendererD3D
{
    /// A specialization of RenderShape that allows for a texture per shape.
    /// Setting the texture per shape is slower than doing it per context but can be 
    /// required while rendering certain shapes.  One example would be a batch of transparent
    /// objects that are depth sorted.
	class RenderShapeSkinned : public RenderShapeWithTexture
	{
	private:
		std::vector<DirectX::XMFLOAT4X4> boneMatrices;

	public:
		/// Initializes the RenderShape with LODs.
		/// \param formHandles - The list of handles to the RenderForm that 
        /// contains the mesh, material, and context used to render this shape 
        /// for each LOD.
		/// \param localMatrixPtr - The optional local transform used to render 
        /// this shape.
        void Initialize(
			std::vector< EDUtilities::ContentHandle<RenderForm> > &formHandles, 
			DirectX::XMFLOAT4X4* localMatrixPtr = 0 );

		void SetBoneMatrices(const std::vector<EDMath::Float4x4> &_boneMatrices);
        /// An EDRenderFunc that can render shapes whose vertices are indexed.
        /// The texture handles will be set in this method, 
        /// which is not as fast as setting them in the context.
		static void IndexedPrimitiveRenderFunc(RenderNode &node);

		static void IndexedPrimitiveRenderFuncSolution(RenderNode &node);

	};
}