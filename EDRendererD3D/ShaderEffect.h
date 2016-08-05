#pragma once




#include "../tinyxml/tinyxml.h"

namespace EDRendererD3D
{
	class ShaderPass
	{
	public:
		/// Loads the vertex shader from a compiled file
		/// \param fileName - the path and name of the file to be loaded, expected extension .cso
		bool CreateVertexShaderFromCompiledFile(const char *fileName);
		/// Loads the pixel shader from a compiled file
		/// \param fileName - the path and name of the file to be loaded, expected extension .cso
		bool CreatePixelShaderFromCompiledFile(const char *fileName);
		/// Loads the geometry shader from a compiled file
		/// \param fileName - the path and name of the file to be loaded, expected extension .cso
		bool CreateGeometryShaderFromCompiledFile(const char *fileName);
		/// Loads the hull shader from a compiled file
		/// \param fileName - the path and name of the file to be loaded, expected extension .cso
		bool CreateHullShaderFromCompiledFile(const char *fileName);
		/// Loads the domain shader from a compiled file
		/// \param fileName - the path and name of the file to be loaded, expected extension .cso
		bool CreateDomainShaderFromCompiledFile(const char *fileName);


		bool LoadPassFromElement(TiXmlElement *passElementPtr);

		inline CComPtr<ID3D11PixelShader>		&GetPixelShader()		{ return pixelShader; }
		inline CComPtr<ID3D11VertexShader>		&GetVertexShader()		{ return vertexShader; }
		inline CComPtr<ID3D11GeometryShader>	&GetGeometryShader()	{ return geometryShader; }
		inline CComPtr<ID3D11HullShader>		&GetHullShader()		{ return hullShader; }
		inline CComPtr<ID3D11DomainShader>		&GetDomainShader()		{ return domainShader; }

		inline const CComPtr<ID3D11PixelShader>		&GetPixelShader() const		{ return pixelShader; }
		inline const CComPtr<ID3D11VertexShader>	&GetVertexShader() const	{ return vertexShader; }
		inline const CComPtr<ID3D11GeometryShader>	&GetGeometryShader() const	{ return geometryShader; }
		inline const CComPtr<ID3D11HullShader>		&GetHullShader() const		{ return hullShader; }
		inline const CComPtr<ID3D11DomainShader>	&GetDomainShader() const		{ return domainShader; }


	private:
		/// The pixel shader managed pointer
		CComPtr<ID3D11PixelShader>		pixelShader;
		/// The vertex shader managed pointer
		CComPtr<ID3D11VertexShader>		vertexShader;
		/// The geometry shader managed pointer
		CComPtr<ID3D11GeometryShader>	geometryShader;
		/// The Hull shader managed pointer
		CComPtr<ID3D11HullShader>		hullShader;
		/// The Hull shader managed pointer
		CComPtr<ID3D11DomainShader>		domainShader;

	};

	class ShaderTechnique
	{
	public:
		ShaderTechnique();
		~ShaderTechnique();

		bool LoadTechniqueFromElement(TiXmlElement *techElementPtr);

		inline ShaderPass *GetPass(unsigned int iPass)
		{
			if(shaderPassesCount <= iPass)
				return nullptr;
			return shaderPasses + iPass;
		}

		inline const ShaderPass *GetPass(unsigned int iPass) const
		{
			if(shaderPassesCount <= iPass)
				return nullptr;
			return shaderPasses + iPass;
		}


		inline const std::string &GetName() { return techniqueName; }

		inline unsigned int GetNumPasses() const { return shaderPassesCount; }
	private:
		ShaderPass *shaderPasses;
		unsigned int shaderPassesCount;
		std::string techniqueName;
	};

	class ShaderEffect
	{
	public:
		ShaderEffect();
		~ShaderEffect();
		static ShaderEffect *LoadXML(const char* xmlFileNamePtr, const char* indentifier = 0);

		inline ShaderTechnique *GetTechnique(unsigned int iTech)
		{
			if(shaderTechiquesCount <= iTech)
				return nullptr;
			return shaderTechiques + iTech;
		}

		/// Gets the index of a particular technique whose name matches what is passed in
		/// \param name - The name of the technique whose index should be returned
		/// \return Returns the index of the technique requested, returns -1 if the technique cannot be found
		int GetTechniqueIndexByName(const std::string name);
		ShaderTechnique *GetTechniqueByName(const std::string name);

		/// Helper method for loading shader byte code into memory
		/// \param byteCode - Pointer to the memory loacation to store shader byte code
		/// \param byteCodeSixe - Out paramter to receive the size of the byte code
		/// \param fileName - the path and name of the file to be loaded, expected extension .cso
		static bool LoadCompiledShaderData(char **byteCode, size_t &byteCodeSize, const char *fileName);
	private:
		ShaderTechnique *shaderTechiques;
		unsigned int shaderTechiquesCount;

		std::string name;
	};
}