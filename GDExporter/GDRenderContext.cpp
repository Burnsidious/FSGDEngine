#include "GDRenderContext.h"

#include "GDExporter.h"

#include <maya/MPlug.h>
#include <maya/MString.h>
#include <maya/MFnMesh.h>

using namespace std;

const char* GDRenderContext::RenderStyleString[GDRS_MAX] = { "DEFERRED", "FORWARD", "TRANSPARENT" };
const char* GDRenderContext::VertexFormatString[GDVF_MAX] = { "VERTEX_POS", "VERTEX_POSCOLOR", "VERTEX_POSTEX", "VERTEX_POSNORMTEX", "VERTEX_POSNORMTANTEX" };

bool GDRenderContext::operator==(const GDRenderContext& rhs)const
{
	if( !CompareStrings( material, rhs.material ) )
		return false;
	if( !CompareStrings( effect, rhs.effect ) )
		return false;
	if( !CompareStrings( technique, rhs.technique ) )
		return false;

	if( renderStyle != rhs.renderStyle )
		return false;
	if( vertexFormat != rhs.vertexFormat )
		return false;
	if( m_CastShadows != rhs.m_CastShadows )
		return false;

	return true;
}

GDRenderContext::GDRenderContext(MPlug &renderContextPlug)
{
	MPlug namePlug = renderContextPlug.child(0);
	MString nameString;
	namePlug.getValue(nameString);
	name = nameString.asChar();

	MPlug matPlug = renderContextPlug.child(1);
	MString matString;
	matPlug.getValue(matString);
	material = matString.asChar();

	MPlug fxPlug = renderContextPlug.child(2);
	MString fxString;
	fxPlug.getValue(fxString);
	effect = fxString.asChar();

	MPlug techPlug = renderContextPlug.child(3);
	MString techString;
	techPlug.getValue(techString);
	technique = techString.asChar();

	MPlug rsPlug = renderContextPlug.child(4);
	short rsShort;
	rsPlug.getValue(rsShort);
	renderStyle = (GDRenderStyle)rsShort;
	
	MPlug vfPlug = renderContextPlug.child(5);
	short vfShort;
	vfPlug.getValue(vfShort);
	vertexFormat = (GDVertexFormat)vfShort;

	MPlug shdwPlug = renderContextPlug.child(6);
	bool shdwBool;
	shdwPlug.getValue(shdwBool);
	m_CastShadows = shdwBool;
}

void GDRenderContext::CopyToMesh(MFnMesh &mesh)
{
	MDagPath meshPath;
	mesh.getPath(meshPath);

	MFnDependencyNode depNode(meshPath.node());
	MStatus fpResult;
	MPlug gdMeshPlug = mesh.findPlug("GDRenderContext", &fpResult);

	if( fpResult == MS::kSuccess )
	{	
		mesh.removeAttribute( gdMeshPlug );
		gdMeshPlug = mesh.findPlug("GDRenderContext", &fpResult);
	}

	if( fpResult != MS::kSuccess )
	{
		MStatus result;
		
		MFnCompoundAttribute rcAttribute;
		MObject newAttributeObj = rcAttribute.create( "GDRenderContext", "GDRcx", &result);
		
		MFnCompoundAttrAddString( rcAttribute, "Name", "Nm", "LitTexturedShadows" );
		MFnCompoundAttrAddString( rcAttribute, "Material", "mt" );
		MFnCompoundAttrAddString( rcAttribute, "Effect", "fx", "LitTextured.fx" );
		MFnCompoundAttrAddString( rcAttribute, "Technique", "tch", "Basic" );

		const char* pRenderStyles[] = { "DEFERRED", "FORWARD", "TRANSPARENT" };
		MFnCompoundAttrAddEnum( rcAttribute, "RenderStyle", "rstyl", pRenderStyles, 3, 0 );

		const char* pVertexFormats[] = { "VERTEX_POS", "VERTEX_POSCOLOR", "VERTEX_POSTEX", "VERTEX_POSNORMTEX", "VERTEX_POSNORMTANTEX" };
		MFnCompoundAttrAddEnum( rcAttribute, "VertexFormat", "vfmt", pVertexFormats, 5, 3 );

		MFnCompoundAttrAddBool( rcAttribute, "CastShadows", "shdw", true );

		mesh.addAttribute( newAttributeObj );

		MPlug rcPlug = mesh.findPlug("GDRenderContext", &result);
		if( result != MS::kSuccess )
			return;

		MPlug namePlug = rcPlug.child(0);
		namePlug.setValue( (name.length() == 0 ? "" : name.c_str()) );

		MPlug matPlug = rcPlug.child(1);
		matPlug.setValue( (material.length() == 0 ? "" : material.c_str()) );

		MPlug fxPlug = rcPlug.child(2);
		fxPlug.setValue( (effect.length() == 0 ? "" : effect.c_str()) );

		MPlug techPlug = rcPlug.child(3);
		techPlug.setValue( (technique.length() == 0 ? "" : technique.c_str()) );

		MPlug rsPlug = rcPlug.child(4);
		rsPlug.setValue( (short)renderStyle );
		
		MPlug vfPlug = rcPlug.child(5);
		vfPlug.setValue( (short)vertexFormat );

		MPlug shdwPlug = rcPlug.child(6);
		shdwPlug.setValue(m_CastShadows);
	}
}

void GDRenderContext::WriteXML(void)
{
	const char* pExportPath = GDExporter::GetInstance()->EstablishExportPath();

	if( pExportPath == 0 )
		return;

	if( name.length() == 0 )
		return;
	if( effect.length() == 0 )
		return;

	std::string filePath = pExportPath;
	filePath += "/GDRenderContext";
	_mkdir( filePath.c_str() );
	filePath += "/rcx_";
	filePath += name;
	filePath += ".xml";
	if( !GDExporter::GetInstance()->ValidateFileCreation(filePath.c_str()) )
		return;

	ofstream fout( filePath.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
	
	fout << "<?xml version=\"1.0\"?>\n";
	fout << "<GDRenderContext name=\"" << name.c_str() << "\">\n";
	fout << "<Material>";
	if( material.length() != 0 )
		fout << material.c_str();
	fout << "</Material>\n";
	fout << "<Effect>" << effect.c_str() << "</Effect>\n";
	fout << "<Technique>";
	if( technique.length() != 0 )
		fout << technique.c_str();
	fout << "</Technique>\n";
	fout << "<VertexFormat>" << VertexFormatString[vertexFormat] << "</VertexFormat>\n";
	fout << "<CastShadows>" << (m_CastShadows? "true" : "false") << "</CastShadows>\n";
	fout << "<RenderStyle>" << RenderStyleString[renderStyle] << "</RenderStyle>\n";
	fout << "</GDRenderContext>";

	fout.close();
}