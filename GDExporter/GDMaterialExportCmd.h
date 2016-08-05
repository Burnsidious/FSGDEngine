#pragma once

#include <string>
#include <maya/MPxCommand.h>
#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <list>

enum GDMaterialProperties{ GDM_AMBIENT = 0, GDM_DIFFUSE, GDM_SPECULAR, GDM_EMMISIVE, GDM_REFLECTED, GDM_NORMAL };

struct GDMaterial
{
	std::string m_MaterialFilePath;
	std::string m_MaterialName;
	std::string m_TextureFileNames[GDM_NORMAL+1];
	float m_MaterialColors[GDM_NORMAL][4];

	void WriteXML(std::ofstream &fout);

	GDMaterial(void)
	{
		memset( m_MaterialColors, 0, sizeof(float)*(GDM_NORMAL*4) );
		for(unsigned int i = 0; i < GDM_NORMAL; ++i)
			m_MaterialColors[i][3] = 1.0f;
		
	}

	bool operator==(const GDMaterial& mat)
	{
		//const char* matNameLH = m_MaterialName.length() == 0 ? 0 : m_MaterialName.c_str();
		//const char* matNameRH = mat.m_MaterialName.length() == 0 ? 0 : mat.m_MaterialName.c_str();

		//if( matNameLH == 0 && matNameRH == 0 )
		//	return true;
		//if( matNameLH != 0 && matNameRH == 0 )
		//	return false;
		//if( matNameLH == 0 && matNameRH != 0 )
		//	return false;

		//if( (strcmp( matNameLH, matNameRH ) == 0 ) )
		//	return false;

		for(unsigned int i = 0; i < GDM_NORMAL+1; ++i)
		{
			const char* pLHS = m_TextureFileNames[i].length() == 0 ? 0 : m_TextureFileNames[i].c_str();
			const char* pRHS = mat.m_TextureFileNames[i].length() == 0 ? 0 : mat.m_TextureFileNames[i].c_str();
			
			if( pLHS == 0 && pRHS == 0 )
				continue;
			if( pLHS != 0 && pRHS == 0 )
				continue;
			if( pLHS == 0 && pRHS != 0 )
				continue;

			if( !(strcmp( pLHS, pRHS ) == 0 ) )
				return false;
		}

		for(unsigned int i = 0; i < GDM_NORMAL; ++i)
			for(unsigned int j = 0; j < 4; ++j)
				if( abs( m_MaterialColors[i][j] - mat.m_MaterialColors[i][j] ) > 0.001f )
					return false;

		return true;
	}
};

class GDMaterialExportCmd : public MPxCommand
{
	GDMaterial m_Material;

	std::list<GDMaterial> m_UniqueMaterials;

	/// Copies and converts a texture to a usable place and location for our content pipeline
	/// All textures that are not of the DDS format will be converted to DDS
	/// \param fullPathTextureName The full path name of the texture to be exported
	/// /return 
	int ExportTexture(MString &fullPathTextureName);

public:
	GDMaterialExportCmd(void){}
	~GDMaterialExportCmd(){}

	void Reset(void)
	{
		m_Material.m_MaterialFilePath.clear();
		m_Material.m_MaterialName.clear();

		memset( m_Material.m_MaterialColors, 0, sizeof(float)*4*GDM_NORMAL );
		for(unsigned int i = 0; i < GDM_NORMAL; ++i)
		{
			m_Material.m_TextureFileNames[i].clear();
			m_Material.m_MaterialColors[i][3] = 1.0f;
		}

		m_Material.m_TextureFileNames[GDM_NORMAL].clear();

		m_UniqueMaterials.clear();
	}

	static void *creator(void);
	static MSyntax newSyntax(void);
	MStatus doIt(const MArgList &args);

	void GetMaterialFromMesh(MFnMesh &fn_mesh, GDMaterial &mat);
	void GetTextureFromMaterial(MFnDependencyNode &fn_mat, unsigned int index, GDMaterial &mat);
	void GetNormalTextureFromMaterial(MFnDependencyNode &fn_mat, GDMaterial &mat);

	void GetUniqueMaterials(MFnTransform &transform);
	void ReplaceExtensionWithDDS(std::string &str);
};