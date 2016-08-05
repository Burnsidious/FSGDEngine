#include "RenderTexture2D.h"
#include "../DirectXTK/Inc/DDSTextureLoader.h"
#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/ContentManager.h"
#include "../EDUtilities/PakFile.h"
using namespace EDUtilities;
#include "../EDRendererD3D/Renderer.h"


using std::string;

namespace EDRendererD3D
{
	RenderTexture2D::~RenderTexture2D()
	{
		Destroy();
	}

	void RenderTexture2D::Destroy()
	{
		RenderTexture::Destroy();
	}

	RenderTexture2D *RenderTexture2D::Load(const char* pInFileName)
	{
		string temp = ContentManager::theContentPath;
		temp += pInFileName;
		const char* fileName = temp.c_str();

		InternalOutput::GetReference() << "Loading " << fileName << "...\n";

		WCHAR *buffer = new WCHAR[strlen(fileName)+1];

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fileName, 
			static_cast<int>(strlen(fileName)), buffer, static_cast<unsigned int>(strlen(fileName)));

		buffer[strlen(fileName)] = 0;

		RenderTexture2D *renderTexturePtr = new RenderTexture2D;
		
		// if the asset is in the pak file load from there, if not just try to load from file system
		if (!PakFile::GetInstance()->IsInPak(pInFileName))
		{
			renderTexturePtr->Create(buffer);
		}
		else
		{
			renderTexturePtr->CreateFromPak(fileName);
		}

		delete[] buffer;

		return renderTexturePtr;
	}

	void RenderTexture2D::Create(D3D11_TEXTURE2D_DESC &desc, const char * name)
	{
		Destroy();

		// Create the texture
		HR(Renderer::theDevicePtr->CreateTexture2D(&desc, 0, (ID3D11Texture2D **)&targetTexturePtr));
		SetD3DName(targetTexturePtr, name);
	}

	void RenderTexture2D::Create(const WCHAR *buffer, const char * name)
	{
		Destroy();

		HR(CreateDDSTextureFromFile(Renderer::theDevicePtr, buffer,
			&targetTexturePtr, &textureSRV));
		SetD3DName(targetTexturePtr, name);
	}

	void RenderTexture2D::CreateFromPak(const char* buffer, const char* name)
	{
		char* file_buffer = nullptr;
		unsigned int file_size = 0;

		if (PakFile::GetInstance()->LoadAsset(buffer, &file_buffer, file_size))
		{
			Destroy();
			HR(CreateDDSTextureFromMemory(Renderer::theDevicePtr, (const uint8_t*)file_buffer, file_size, &targetTexturePtr, &textureSRV));
			SetD3DName(targetTexturePtr, name);

			delete [] file_buffer;
			file_buffer = nullptr;
		}
	}

	ID3D11RenderTargetView *RenderTexture2D::GetRenderTargetView(UINT index)
	{
		// Do not try to create a view to an uninitialized resource
		assert(targetTexturePtr);
		if(!textureRTV)
		{
			// Create the requested view
			HR(Renderer::theDevicePtr->CreateRenderTargetView(targetTexturePtr, 0, &textureRTV));
		}
		return textureRTV;
	}

}