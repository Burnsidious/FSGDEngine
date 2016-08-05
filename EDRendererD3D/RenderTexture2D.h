#pragma once



#include "RenderTexture.h"

namespace EDRendererD3D
{
	class RenderTexture2D : public RenderTexture
	{
	protected:
		CComPtr<ID3D11RenderTargetView> textureRTV;
	public:
		RenderTexture2D(void) : RenderTexture(), textureRTV(0) {}
		virtual ~RenderTexture2D();
		void Destroy();
		/// Creates a texture based on the passed in description
		/// \param desc - A description of the desired texture
		void Create(D3D11_TEXTURE2D_DESC &desc, const char * name = 0);
		/// Creates a texture from an asset
		/// \param buffer - A wide character string representing the location of the asset to 
		/// create a texture from
		void Create(const WCHAR *buffer, const char * name = 0);
		// IN buffer - name of the asset to find
		// IN name - name for the d3d asset
		void CreateFromPak(const char* buffer, const char* name = nullptr);
		/// \return Returns the ID3D11ShaderResourceView interface view of this RenderTexture.
		//ID3D11ShaderResourceView *GetShaderResourceView(void);
		/// \return Returns the ID3D11RenderTargetView interface view of this RenderTexture.
		ID3D11RenderTargetView *GetRenderTargetView(UINT index = -1);
		/// Loads a texture from file.
		/// \return Returns the pointer to the RenderTexture.
		/// \param fileName - File path to the texture file to load.
		static RenderTexture2D* Load(const char* fileName);
	};
}