#ifndef _PostProcessWater_H_
#define _PostProcessWater_H_
#include "PostProcess.h"
#include "RenderMaterial.h"
#include "../EDUtilities/Timer.h"
#include "../EDUtilities/ContentManager.h"

namespace EDRendererD3D
{
	/// An object used to implement an image space post-process effect.
	class PostProcessWater : private PostProcess
	{
	private:
		EDUtilities::ContentHandle<RenderMaterial>	renderMaterialHandle;

		// Textures
		//D3DXHANDLE						reflectionMapTextureHandle;
		//D3DXHANDLE						heightMapTextueHandle;
		//D3DXHANDLE						noiseTextureHandle;
		//D3DXHANDLE						foamTextureHandle;
		//D3DXHANDLE						causticTextureHandle;

		//// Colors
		//D3DXHANDLE						surfaceColorHandle;
		//D3DXHANDLE						ambientColorHandle;

		//// Time
		//D3DXHANDLE						elapsedTimeHandle;
		float							elapsedTime;
		EDUtilities::Timer				timer;

	public:
		PostProcessWater(void);
		~PostProcessWater(void);
		/// Creates the effect to be used by this PostProcess.
        /// \param meshPtr - The address of the mesh to be used while rendering this shape.
		/// \param effectFile - a wide character string defining the name of the 
		/// shader to use for this effect.  A leading L makes an in place string
		/// into a wide character one, ex. L"SomeShader.fx"
		/// \return Returns a buffer to any errors that occured while compiling the shader.
		//ID3DXBuffer *Initialize(RenderMesh *_meshPtr, LPCWSTR effectFile);
		/// Loads the XML file to be used by this PostProcess.
        /// \param xmlFileName - The name of the XML file.
		void LoadXML(const char* xmlFileName);
		/// Executes the post-process effect.
		//void Process(RenderTarget &destinationTarget, LPDIRECT3DBASETEXTURE9 sourceTexture);
	};
}

#endif