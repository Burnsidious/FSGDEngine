#pragma once

#include "RenderTexture.h"

namespace EDRendererD3D
{
	class RenderTextureCubeMap : public RenderTexture
	{
	protected:
		DXGI_FORMAT format;

		CComPtr<ID3D11RenderTargetView> textureRTV[6];
		CComPtr<ID3D11RenderTargetView> textureWholeRTV;
	public:
		RenderTextureCubeMap(void);
		virtual ~RenderTextureCubeMap();
		void Destroy();

		/// Creates a texture from an asset
		/// \param buffer - A wide character string representing the location of the asset to 
		/// create a texture from
		void Create(const WCHAR *buffer, const char * name = 0);
		void Create(D3D11_TEXTURE2D_DESC &desc, const char * name = 0);

		/// \return Returns the ID3D11ShaderResourceView interface view of this RenderTexture.
		ID3D11ShaderResourceView *GetShaderResourceView();
		/// \return Returns the ID3D11RenderTargetView interface view of this RenderTexture.
		ID3D11RenderTargetView *GetRenderTargetView(UINT index = -1);

		/// Loads a texture from file.
		/// \return Returns the pointer to the RenderTextureCubeMap.
		/// \param fileName - File path to the texture file to load.
		static RenderTextureCubeMap* Load(const char* fileName);
	};
}
