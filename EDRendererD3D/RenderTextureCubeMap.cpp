#include "precompiled.h"
#include "RenderTextureCubeMap.h"
#include "../DirectXTK/Inc/DDSTextureLoader.h"

#include "../EDRendererD3D/Renderer.h"
#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/ContentManager.h"

using namespace EDUtilities;
using namespace std;

namespace EDRendererD3D
{
	RenderTextureCubeMap::RenderTextureCubeMap(void) : RenderTexture()
	{
		for(UINT i = 0; i < 6; ++i)
		{
			textureRTV[i] = nullptr;
		}
		textureWholeRTV = nullptr;
	}

	RenderTextureCubeMap::~RenderTextureCubeMap()
	{
		Destroy();
	}

	void RenderTextureCubeMap::Destroy()
	{
		RenderTexture::Destroy();
	}

	RenderTextureCubeMap* RenderTextureCubeMap::Load(const char* pInFileName)
	{
		string temp = ContentManager::theContentPath;
		temp += pInFileName;
		const char* fileName = temp.c_str();

		InternalOutput::GetReference() << "Loading " << fileName << "...\n";

		WCHAR *buffer = new WCHAR[strlen(fileName)+1];

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fileName, 
			static_cast<int>(strlen(fileName)), buffer, static_cast<int>(strlen(fileName)));

		buffer[strlen(fileName)] = 0;

		RenderTextureCubeMap* pRenderTextureCubeMap = new RenderTextureCubeMap;
		pRenderTextureCubeMap->Create(buffer);

		delete[] buffer;

		return pRenderTextureCubeMap;	
		return 0;
	}

	void RenderTextureCubeMap::Create(D3D11_TEXTURE2D_DESC &desc, const char * name)
	{
		Destroy();

		// Create the texture
		format = desc.Format;
		HR(Renderer::theDevicePtr->CreateTexture2D(&desc, 0, (ID3D11Texture2D **)&targetTexturePtr));
		SetD3DName(targetTexturePtr, name);
	}

	void RenderTextureCubeMap::Create(const WCHAR *buffer, const char * name)
	{
		Destroy();

		HR(CreateDDSTextureFromFile(Renderer::theDevicePtr, buffer, 
			&targetTexturePtr, &textureSRV));
		SetD3DName(targetTexturePtr, name);
	}

	ID3D11ShaderResourceView *RenderTextureCubeMap::GetShaderResourceView()
	{ 
		// Do not try to create a view to an uninitialized resource
		assert(targetTexturePtr);
		if(!textureSRV)
		{
			// Create the requested view
			D3D11_SHADER_RESOURCE_VIEW_DESC rViewDesc;
			ZeroMemory( &rViewDesc, sizeof(rViewDesc) );
			rViewDesc.Format = format;
			rViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			rViewDesc.TextureCube.MipLevels = -1;
			rViewDesc.TextureCube.MostDetailedMip = 0;
			HR(Renderer::theDevicePtr->CreateShaderResourceView(targetTexturePtr, &rViewDesc, &textureSRV));
		}
		return textureSRV; 
	}

	ID3D11RenderTargetView *RenderTextureCubeMap::GetRenderTargetView(UINT index)
	{
		// Do not try to create a view to an uninitialized resource
		assert(targetTexturePtr);

		// Give the RTV for all six faces
		if(-1 == index)
		{
			if(!textureWholeRTV)
			{
				// Create the requested view
				D3D11_RENDER_TARGET_VIEW_DESC tViewDesc;
				tViewDesc.Format = format;
				tViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;

				tViewDesc.Texture2DArray.FirstArraySlice = 0;
				tViewDesc.Texture2DArray.ArraySize = 6;
				tViewDesc.Texture2DArray.MipSlice = 0;

				HR(Renderer::theDevicePtr->CreateRenderTargetView(targetTexturePtr, &tViewDesc, &textureWholeRTV));
			}
			return textureWholeRTV;

		}
		// Give the RTV for one face
		else
		{
			if(!textureRTV[0])
			{
				// Create the requested view
				D3D11_RENDER_TARGET_VIEW_DESC tViewDesc;
				tViewDesc.Format = format;
				tViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				tViewDesc.Texture2DArray.ArraySize = 1;
				tViewDesc.Texture2DArray.MipSlice = 0;
				for(UINT i = 0; i < 6; ++i)
				{
					tViewDesc.Texture2DArray.FirstArraySlice = i;
					HR(Renderer::theDevicePtr->CreateRenderTargetView(targetTexturePtr, &tViewDesc, &textureRTV[i]));
				}
			}
			return textureRTV[index];
		}
	}
}