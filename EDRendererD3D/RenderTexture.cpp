#include "precompiled.h"
#include "RenderTexture.h"
#include "../DirectXTK/Inc/DDSTextureLoader.h"

#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;
#include "../EDRendererD3D/Renderer.h"


namespace EDRendererD3D
{
	RenderTexture::~RenderTexture()
	{
		Destroy();
	}

	void RenderTexture::Destroy()
	{
	}

	void RenderTexture::Create(const WCHAR *buffer, const char * name)
	{
		Destroy();

		HR(CreateDDSTextureFromFile(Renderer::theDevicePtr, buffer, 
			&targetTexturePtr, &textureSRV));
		SetD3DName(targetTexturePtr, name);
	}

	ID3D11ShaderResourceView *RenderTexture::GetShaderResourceView(void)
	{ 
		// Do not try to create a view to an uninitialized resource
		assert(targetTexturePtr);
		if(!textureSRV)
		{
			// Create the requested view
			HR(Renderer::theDevicePtr->CreateShaderResourceView(targetTexturePtr, 0, &textureSRV));
		}
		return textureSRV; 
	}
}