#pragma once

#include <string>
#include <direct.h>
#include <list>

#include <maya/MGlobal.h>
#include <maya/MStringArray.h>
#include <maya/MFnMesh.h>
#include <maya/MSyntax.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MFnTransform.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MColor.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnData.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnEnumAttribute.h>


#include <iostream>
#include <fstream>

#include "GDRenderContext.h"

bool CompareStrings(const std::string &lhs, const std::string &rhs);

class GDExporter
{
	GDExporter(void){}
	GDExporter(const GDExporter&){}
	GDExporter& operator=(const GDExporter&){ return *this; }
	static GDExporter* instancePtr;

	std::string m_ExportPath;

	void SetExportPath(const char* path)
	{
		if( path == 0 )
			m_ExportPath = "";
		else
			m_ExportPath = path;
	}

public:
	~GDExporter(void){}

	static GDExporter* GetInstance(void)
	{
		if( instancePtr == 0 )
			instancePtr = new GDExporter();

		return instancePtr;
	}

	static void DeleteInstance(void)
	{
		delete instancePtr;
		instancePtr = 0;
	}

	const char* GetExportPath(void)
	{
		if( m_ExportPath.length() == 0 )
			return 0;

		return m_ExportPath.c_str();
	}

	void ReestablishExportPath(void)
	{
		MStringArray exportPath;
		MGlobal::executeCommand( "fileDialog2 -ds 2 -cap \"GDExporter Export Path\" -fm 3", exportPath );
		
		if( exportPath.length() != 0 )
			SetExportPath( exportPath[0].asChar() );
		else
			SetExportPath( 0 );
	}

	const char* EstablishExportPath(void)
	{
		if( GetExportPath() == 0 )
			ReestablishExportPath();

		return GetExportPath();
	}

	bool ValidateFileCreation(const char* fileName)
	{
		bool bResult = true;
		std::ifstream fin(fileName);
		fin.close();
	
		if( !fin.fail() )
		{
			MString commandResult;

			MString confirmString = "confirmDialog -title \"Confirm Overwrite\" -button \"Okay\" -button \"Cancel\" -cancelButton \"Cancel\" -message ";
			confirmString += "\"Overwrite ";
			confirmString += fileName;
			confirmString += "?\"";

			MGlobal::executeCommand( confirmString, commandResult );

			if( commandResult == "Cancel" )
				bResult = false;
		}

		fin.clear(std::ios_base::failbit);

		return bResult;
	}

	MFnMesh* GetSelectedMesh(void);

	bool GetSelectedMeshTransformPath(MDagPath &transformPath);

	const char* GetSceneName(std::string &sceneName);

	void GetUniqueRenderContexts( MFnTransform& currentTransform, std::list<GDRenderContext>& uniqueContexts );
};

void MFnCompoundAttrAddString( MFnCompoundAttribute &attr, const char* full, const char* brief, const char* def = 0 );

void MFnCompoundAttrAddBool( MFnCompoundAttribute &attr, const char* full, const char* brief, bool def = false );

void MFnCompoundAttrAddEnum( MFnCompoundAttribute &attr, const char* full, const char* brief, const char** pEnumStrings, unsigned short count, unsigned short def = 0 );