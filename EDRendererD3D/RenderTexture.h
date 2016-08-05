#pragma once



namespace EDRendererD3D
{
	class RenderTexture
	{
	protected:
        CComPtr<ID3D11Resource> targetTexturePtr;
		CComPtr<ID3D11ShaderResourceView> textureSRV;
	public:
		RenderTexture(void) : textureSRV(0), targetTexturePtr(0) {}
		virtual ~RenderTexture() = 0;
		virtual void Destroy();
		/// Creates a texture based on the passed in description
		/// \param desc - A description of the desired texture
		virtual void Create(D3D11_TEXTURE2D_DESC &desc, const char * name = 0) = 0;
		/// Creates a texture from an asset
		/// \param buffer - A wide character string representing the location of the asset to 
		/// create a texture from
		virtual void Create(const WCHAR *buffer, const char * name = 0);
		/// \return Returns the ID3D11ShaderResourceView interface view of this RenderTexture.
		virtual ID3D11ShaderResourceView *GetShaderResourceView(void);
		/// \return Returns the ID3D11RenderTargetView interface view of this RenderTexture.
		virtual ID3D11RenderTargetView *GetRenderTargetView(UINT index = -1) = 0;

		CComPtr<ID3D11Resource> GetTextureResource() { return targetTexturePtr; };
		/// Loads a texture from file.
		/// \return Returns the pointer to the RenderTexture.
		/// \param fileName - File path to the texture file to load.
		//static RenderTexture* Load(const char* fileName) = 0;
	};
}