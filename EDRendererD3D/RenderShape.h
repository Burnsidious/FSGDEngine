#pragma once

#include "RenderShapeBase.h"


namespace EDRendererD3D
{
	/// RenderShape is a specialization of RenderNode used to define a renderable object.
	class RenderShape : public RenderShapeBase
	{
	protected:

		/// Handle to this shape's RenderForms, if loaded from file.
		/// Each handle represents a different LOD with the highest resolution 
		/// being first.
		std::vector< EDUtilities::ContentHandle<RenderForm> > renderFormHandles;

		DirectX::XMFLOAT4X4 worldMatrix;

		float zDepth;

		EDMath::Sphere sphere;

		unsigned int renderNumber;

		unsigned int currentLOD;

	public:
		RenderShape() : renderNumber(0), currentLOD(0) {}

		virtual ~RenderShape() {	}

		/// Initializes the RenderShape with LODs.
		/// \param formHandles - The list of handles to the RenderForm that 
		/// contains the mesh, material, and context used to render this shape 
		/// for each LOD.
		/// \param localMatrixPtr - The optional local transform used to render 
		/// this shape.
		virtual void Initialize(
			std::vector< EDUtilities::ContentHandle<RenderForm> > &formHandles,
			DirectX::XMFLOAT4X4 *localMatrixPtr = 0 );

		/// A helper function to simplify adding shapes to their RenderSets.
		/// This method will add the shape to the RenderSet held by the shapes context.
		/// This is the way to add this shape to a render batch.
		void AddToContextSet(RenderContext *contextPtr = nullptr);

		/// \return Returns a bounding sphere that encloses this shape.
		const EDMath::Sphere& GetSphere(void);
		const EDMath::Sphere& GetLocalSphere(void) const;

		inline void SetWorldMatrix(const DirectX::XMFLOAT4X4 &mat) { worldMatrix = mat; }
		inline void SetWorldMatrix(const DirectX::XMFLOAT4X4 *mat) { worldMatrix = *mat; }
		inline DirectX::XMFLOAT4X4 &GetWorldMatrix(void) { return worldMatrix; }
		inline DirectX::XMFLOAT4X4 *GetWorldMatrixPtr(void) { return &worldMatrix; }

		inline void SetZDepth(float _zDepth) { zDepth = _zDepth; }
		inline float GetZDepth(void) const { return zDepth; }

		//void SetRenderFormHandle(ContentHandle<RenderForm> &handle);
		inline void SetRenderFormHandle(
			std::vector< EDUtilities::ContentHandle<RenderForm> > &handles)
		{
			renderFormHandles = handles;
		}

		std::vector< EDUtilities::ContentHandle<RenderForm> > &GetRenderFormHandles()
		{
			return renderFormHandles;
		}

		inline const RenderContext *GetRenderContext(int lod) const 
		{
			if(lod == -1)
				lod = currentLOD;
			return renderFormHandles[lod].GetContent()->GetRenderContextHandle().GetContent();
		}
		inline RenderContext *GetRenderContext(int lod) 
		{ 
			if(lod == -1)
				lod = currentLOD;
			return renderFormHandles[lod].GetContent()->GetRenderContextHandle().GetContent();
		}
		inline const RenderContext *GetRenderContext() const 
		{
			return renderFormHandles[currentLOD].GetContent()->GetRenderContextHandle().GetContent();
		}
		inline RenderContext *GetRenderContext() 
		{ 
			return renderFormHandles[currentLOD].GetContent()->GetRenderContextHandle().GetContent();
		}

		inline RenderMesh *GetRenderMesh(int lod = -1) const
		{ 
			if(lod == -1)
				lod = currentLOD;
			return renderFormHandles[lod].GetContent()->GetMeshHandle().GetContent();
		}

		inline size_t GetNumLODs(void) { return renderFormHandles.size(); }
		inline void SetCurrentLOD(unsigned int lod)
		{ 
			currentLOD = lod;
		}
		inline size_t GetCurrentLOD(void) { return currentLOD; }

		/// An EDRenderFunc that does nothing.  
		/// This can be used as a placeholder or in cases when we do not want shapes in view to render.
		static void DoNotRender(RenderNode &node) {}
		/// An EDRenderFunc that can render shapes whose vertices are indexed.
		static void IndexedPrimitiveRenderFunc(RenderNode &node);

		/// Solution method declaration.  Body contained in solutions static library
		static void IndexedPrimitiveRenderFuncSolution(RenderNode &node);

	};
}