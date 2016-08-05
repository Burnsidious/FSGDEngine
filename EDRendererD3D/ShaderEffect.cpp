#include "precompiled.h"
using std::ifstream;
using std::ios_base;

#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;

#include "ShaderEffect.h"
#include "Renderer.h"

namespace EDRendererD3D
{
	//////////////////////////////////////////////////////////////////
	// Shader Pass methods
	bool ShaderPass::CreateVertexShaderFromCompiledFile(const char *fileName)
	{
		char *byteCode;
		size_t byteCodeSize;
		if(!ShaderEffect::LoadCompiledShaderData(&byteCode, byteCodeSize, fileName))
			return false;

		HRESULT hr = Renderer::theDevicePtr->CreateVertexShader(
			byteCode, byteCodeSize, nullptr, &vertexShader);
		
		delete [] byteCode;
		
		if(S_OK != hr)
		{
			HR(hr);
			return false;
		}

		return true;
	}

	bool ShaderPass::CreatePixelShaderFromCompiledFile(const char *fileName)
	{
		char *byteCode;
		size_t byteCodeSize;
		if(!ShaderEffect::LoadCompiledShaderData(&byteCode, byteCodeSize, fileName))
			return false;

		HRESULT hr = Renderer::theDevicePtr->CreatePixelShader(
			byteCode, byteCodeSize, nullptr, &pixelShader);
		
		delete [] byteCode;

		if(S_OK != hr)
		{
			HR(hr);
			return false;
		}

		return true;
	}

	bool ShaderPass::CreateGeometryShaderFromCompiledFile(const char *fileName)
	{
		char *byteCode;
		size_t byteCodeSize;
		if(!ShaderEffect::LoadCompiledShaderData(&byteCode, byteCodeSize, fileName))
			return false;

		HRESULT hr = Renderer::theDevicePtr->CreateGeometryShader(
			byteCode, byteCodeSize, nullptr, &geometryShader);
		
		delete [] byteCode;

		if(S_OK != hr)
		{
			HR(hr);
			return false;
		}

		return true;
	}

	bool ShaderPass::CreateHullShaderFromCompiledFile(const char *fileName)
	{
		char *byteCode;
		size_t byteCodeSize;
		if(!ShaderEffect::LoadCompiledShaderData(&byteCode, byteCodeSize, fileName))
			return false;

		HRESULT hr = Renderer::theDevicePtr->CreateHullShader(
			byteCode, byteCodeSize, nullptr, &hullShader);
		
		delete [] byteCode;

		if(S_OK != hr)
		{
			HR(hr);
			return false;
		}

		return true;
	}

	bool ShaderPass::CreateDomainShaderFromCompiledFile(const char *fileName)
	{
		char *byteCode;
		size_t byteCodeSize;
		if(!ShaderEffect::LoadCompiledShaderData(&byteCode, byteCodeSize, fileName))
			return false;

		HRESULT hr = Renderer::theDevicePtr->CreateDomainShader(
			byteCode, byteCodeSize, nullptr, &domainShader);
		
		delete [] byteCode;

		if(S_OK != hr)
		{
			HR(hr);
			return false;
		}

		return true;
	}

	bool ShaderPass::LoadPassFromElement(TiXmlElement *passElementPtr)
	{ 
		// Vertex Shader - required
		TiXmlElement *shaderElementPtr = passElementPtr->FirstChildElement("VertexShader");
		if(nullptr == shaderElementPtr)
		{
			InternalOutput::GetReference().Error(
				"Failed to load shader, no VertexShader element found?\n");
			return false;
		}
		if(!CreateVertexShaderFromCompiledFile(shaderElementPtr->GetText()))
		{
			InternalOutput::GetReference().Error(
				"Failed to load vertex shader %s, is the file present?\n", shaderElementPtr->GetText());
			return false;
		}

		// Pixel Shader - required
		shaderElementPtr = passElementPtr->FirstChildElement("PixelShader");
		if(nullptr == shaderElementPtr)
		{
			InternalOutput::GetReference().Error(
				"Failed to load shader, no PixelShader element found?\n");
			return false;
		}
		if(!CreatePixelShaderFromCompiledFile(shaderElementPtr->GetText()))
		{
			InternalOutput::GetReference().Error(
				"Failed to load pixel shader %s, is the file present?\n", shaderElementPtr->GetText());
			return false;
		}

		// Geometry Shader - optional
		shaderElementPtr = passElementPtr->FirstChildElement("GeometryShader");
		if(shaderElementPtr)
		{
			if(!CreateGeometryShaderFromCompiledFile(shaderElementPtr->GetText()))
			{
				InternalOutput::GetReference().Error(
					"Failed to load geometry shader %s, is the file present?\n", shaderElementPtr->GetText());
				return false;
			}
		}
		// Hull Shader - optional
		shaderElementPtr = passElementPtr->FirstChildElement("HullShader");
		if(shaderElementPtr)
		{
			if(!CreateHullShaderFromCompiledFile(shaderElementPtr->GetText()))
			{
				InternalOutput::GetReference().Error(
					"Failed to load hull shader %s, is the file present?\n", shaderElementPtr->GetText());
				return false;
			}
		}

		// Domain Shader - optional
		shaderElementPtr = passElementPtr->FirstChildElement("DomainShader");
		if(shaderElementPtr)
		{
			if(!CreateDomainShaderFromCompiledFile(shaderElementPtr->GetText()))
			{
				InternalOutput::GetReference().Error(
					"Failed to load domain domain %s, is the file present?\n", shaderElementPtr->GetText());
				return false;
			}
		}

		return true; 
	}

	//////////////////////////////////////////////////////////////////
	// Shader Technique methods
	ShaderTechnique::ShaderTechnique()
	{
		shaderPasses = nullptr;
		shaderPassesCount = 0;
	}

	ShaderTechnique::~ShaderTechnique()
	{
		delete [] shaderPasses;
	}

	bool ShaderTechnique::LoadTechniqueFromElement(TiXmlElement *techElementPtr)
	{
		techniqueName = techElementPtr->Attribute("name");
		TiXmlElement *passElementPtr = techElementPtr->FirstChildElement("Pass");
		unsigned int passCount = 0;
		while(nullptr != passElementPtr)
		{
			++passCount;
			passElementPtr = passElementPtr->NextSiblingElement("Pass");
		}

		if(0 == passCount)
			return false;

		shaderPassesCount = passCount;
		shaderPasses = new ShaderPass[shaderPassesCount];

		passElementPtr = techElementPtr->FirstChildElement("Pass");
		unsigned int passIndex = 0;
		while(nullptr != passElementPtr)
		{
			// Load the current technique
			if(!shaderPasses[passIndex++].LoadPassFromElement(passElementPtr))
			{
				delete [] shaderPasses;
				return false;
			}

			passElementPtr = passElementPtr->NextSiblingElement("Pass");
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////
	// Shader Effect methods
	ShaderEffect::ShaderEffect()
	{
		shaderTechiques = nullptr;
		shaderTechiquesCount = 0;
	}

	ShaderEffect::~ShaderEffect()
	{
		delete [] shaderTechiques;
	}

	ShaderEffect *ShaderEffect::LoadXML(const char* xmlFileNamePtr, const char* indentifier)
	{
		std::string fullPath = ContentManager::theContentPath + xmlFileNamePtr;
		TiXmlDocument doc(fullPath.c_str());

		if(!doc.LoadFile())
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file exist?\n", xmlFileNamePtr);
			return 0;
		}
		TiXmlHandle hDoc(&doc);
		TiXmlElement* elementPtr;

		elementPtr = hDoc.FirstChildElement().Element();
		if( !elementPtr )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, is the file empty?\n", xmlFileNamePtr);
			return nullptr;
		}

		unsigned int techniqueCount = 0;
		TiXmlElement *techElementPtr = elementPtr->FirstChildElement("Technique");
		while(nullptr != techElementPtr)
		{
			++techniqueCount;
			techElementPtr = techElementPtr->NextSiblingElement("Technique");
		}

		if(0 == techniqueCount)
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file contain any techniques?\n", xmlFileNamePtr);

			return nullptr;
		}

		ShaderEffect *effectPtr = new ShaderEffect;
		effectPtr->shaderTechiquesCount = techniqueCount;
		effectPtr->shaderTechiques = new ShaderTechnique[effectPtr->shaderTechiquesCount];

		techElementPtr = elementPtr->FirstChildElement("Technique");
		unsigned int techIndex = 0;
		while(nullptr != techElementPtr)
		{
			// Load the current technique
			if(!effectPtr->shaderTechiques[techIndex++].LoadTechniqueFromElement(techElementPtr))
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, could not load techniques\n", xmlFileNamePtr);
				delete effectPtr;
				return 0;
			}
			techElementPtr = techElementPtr->NextSiblingElement("Technique");
		}
		effectPtr->name = xmlFileNamePtr;

		return effectPtr;
	}	

	int ShaderEffect::GetTechniqueIndexByName(const std::string name)
	{
		int ret = -1;

		for(unsigned int i = 0; i < shaderTechiquesCount; ++i)
		{
			if(name == shaderTechiques[i].GetName())
			{
				ret = i;
				break;
			}
		}

		return ret;
	}

	ShaderTechnique *ShaderEffect::GetTechniqueByName(const std::string name)
	{
		ShaderTechnique * ret = nullptr;

		for(unsigned int i = 0; i < shaderTechiquesCount; ++i)
		{
			if(name == shaderTechiques[i].GetName())
			{
				ret = shaderTechiques + i;
				break;
			}
		}

		return ret;
	}

	bool ShaderEffect::LoadCompiledShaderData(char **byteCode, size_t &byteCodeSize, const char *fileName)
	{
		ifstream load; 	
		load.open(fileName, ios_base::binary);
		if(!load.is_open())
			return false;
		load.seekg( 0, ios_base::end ); 
		byteCodeSize = size_t(load.tellg());
		*byteCode = new char[byteCodeSize];
		load.seekg( 0, ios_base::beg ); 
		load.read( *byteCode, byteCodeSize );
		load.close();

		return true;
	}
}
