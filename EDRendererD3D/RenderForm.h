#pragma once

#include "RenderContext.h"
#include "RenderMesh.h"
#include "RenderMaterial.h"
#include "../EDUtilities/ContentManager.h"

namespace EDRendererD3D
{
	class RenderShapeBase;
	/// The types of Render Shapes that could be used for any given form
	enum RenderShapeTypes { RST_Basic, RST_WithMaterial, RST_Target, RST_GUI, RST_DEBUG_LINES, 
		RST_Light, RST_Skinned }; 
	/// Implementation of a RenderForm. A RenderForm has handles to a mesh, a material, and a 
	/// rendercontext.
	/// Collections of RenderForms with matrix and hierarchy information make up models.
	class RenderForm
	{
		EDUtilities::ContentHandle<RenderMesh> renderMeshHandle;
		EDUtilities::ContentHandle<RenderMaterial> renderMaterialHandle;
		EDUtilities::ContentHandle<RenderContext> renderContextHandle;

		RenderShapeTypes renderShapeType;
	public:
		RenderForm(void){}
		~RenderForm(){}

		RenderShapeTypes GetRenderShapeType() { return renderShapeType; }

		bool hasTexture(void);
		/// Loads a RenderForm from a specified XML file.
		/// \param xmlFileNamePtr - File path to the RenderForm to load from XML format.
		/// \param indentifier - A unique identifier, unused in this context. Exists only to match 
		///							function prototype.
		static RenderForm* LoadXML(const char* xmlFileNamePtr, const char* indentifier);

		/// \return Returns the handle to the form's RenderMesh
		inline EDUtilities::ContentHandle<RenderMesh>& GetMeshHandle(void) 
		{ return renderMeshHandle; }
		/// \return Returns the handle to the form's RenderMaterial
		inline EDUtilities::ContentHandle<RenderMaterial>& GetMaterialHandle(void) 
		{ return renderMaterialHandle; }
		/// \return Returns the handle to the form's RenderContext
		inline EDUtilities::ContentHandle<RenderContext>& GetRenderContextHandle(void) 
		{ return renderContextHandle; }

		/// A method for getting a dynamic render shape of the appropriate type for the form
		/// \return Returns the new render shape fo the proper type
		RenderShapeBase *GetNewRenderShape();
	};
}