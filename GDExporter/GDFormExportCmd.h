#pragma once

#include <string>
#include <list>
#include <maya/MPxCommand.h>
#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <maya/MDagPath.h>

struct GDForm
{
	std::string m_FormName;
	std::string m_Mesh;
	std::string m_Material;
	std::string m_RenderContext;

	void WriteXML(std::ofstream &fout);
};

class GDFormExportCmd : public MPxCommand
{
	GDForm m_Form;

	std::list<MDagPath> m_ExportedPaths;
public:
	GDFormExportCmd(void){}
	~GDFormExportCmd(){}

	void Reset(void)
	{
		m_Form.m_FormName.clear();
		m_Form.m_Mesh.clear();
		m_Form.m_Material.clear();
		m_Form.m_RenderContext.clear();

		m_ExportedPaths.clear();
	}
	
	static void* creator(void);
	static MSyntax newSyntax(void);
	MStatus doIt(const MArgList &args);

	void ExportForms(MFnTransform &transform);
};