#pragma once



#include "../EDUtilities/ContentManager.h"
#include "RenderTexture2D.h"
#include "RenderTextureCubeMap.h"

namespace EDRendererD3D
{
	/// Representation for a material color.
	union RenderColor
	{
		struct
		{
			float Red;
			float Green;
			float Blue;
			float Alpha;
		};

		float RGBA[4];
	};
    enum RenderTextureTypes { RTT_Ambient, RTT_Diffuse, RTT_Specular, RTT_Emmisive, 
		RTT_Normal, RTT_Reflected, RTT_Count};
	/// Implementation of a surface material, with different colors and/or texture
	/// maps for surface properties (diffuse, specular, normal, etc.)
	class RenderMaterial
	{
	private:
		std::vector<EDUtilities::ContentHandle<RenderTexture2D> > renderTextureHandles;
		EDUtilities::ContentHandle<RenderTextureCubeMap> renderTextureCubeMapHandle;
		std::vector<RenderColor> renderColors;
	public:
		RenderMaterial(void){}
		
		/// \return Returns the collection of RenderTexture handles for this material.
		std::vector<EDUtilities::ContentHandle<RenderTexture2D> >& GetRTHandles(void)
		{ return renderTextureHandles; }
		/// \return Returns the RenderTextureCubeMap handle for this material.
		EDUtilities::ContentHandle<RenderTextureCubeMap>& GetRTCubeMapHandle(void)
		{ return renderTextureCubeMapHandle; }
		/// \return Returns the collection of RenderColors for this material.
		std::vector<RenderColor>& GetRenderColors(void){ return renderColors; }

		bool hasTexture(void);
		/// Loads a RenderMaterial from a specified XML file.
		/// \param xmlFileNamePtr - File path to the RenderMaterial to load from XML format.
		/// \param indentifier - A unique identifier, unused in this context. Exists only to match function prototype.
		static RenderMaterial* LoadXML(const char* xmlFileNamePtr, const char* indentifier = 0);
	};
}