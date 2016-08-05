#pragma once

#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>
#include <maya/MPlug.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MDagPath.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MItGeometry.h>
#include <vector>
#include <list>

#include "GDExporter.h"
#include "GDVertex.h"
#include "GDTriangle.h"
#include "GDMeshObject.h"

//#include "GDMeshBVH.h"

//#include "../EDTree/AabbBvh.h"
//class AabbBvh;

//struct GDMeshObject
//{
//	std::string meshFilePath;
//	std::string meshName;
//	std::vector<int> indices;
//
//	bool attributePresent[GDVAT_MAX];
//	std::string attributeNames[GDVAT_MAX];
//	std::vector<GDVertex> vertices;
//
//	bool triAttributePresent[GDTAT_MAX];
//	std::string triAttributeNames[GDTAT_MAX];
//	std::vector<GDTriangle> triangles;
//	
//	//AabbBvh* m_pAabbBvh;
//	//GDMeshBVH* m_pAabbBvh;
//
//	//GDMeshObject(void) : m_pAabbBvh(0) {}
//};

struct GDVertexAttributeSet
{
	std::string m_AttributeName;
	std::vector<GDVertexAttribute> m_Attributes;
};

struct GDTriangleAttributeSet
{
	std::string m_AttributeName;
	std::vector<GDTriangleAttribute> m_Attributes;
};

void CollectTexCoordAttribute( const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets, unsigned int setNumber );

template<GDVertexAttributeType gdvat>
void CollectAttribute(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets){}

template<GDTriangleAttributeType gdtat>
void CollectTriAttribute(const MFnMesh& mfnMesh, GDTriangleAttributeSet* pAttributeSets){}

template<GDVertexAttributeType gdvat>
unsigned int AttributeByteCount(void)
{
	return 12;
}

template<GDTriangleAttributeType gdtat>
unsigned int TriAttributeByteCount(void)
{
	return 12;
}

template<GDVertexAttributeType gdvat>
void WriteAttributeXML(std::ofstream &fout, GDVertex &vert)
{
	char buffer[128];
	sprintf_s( buffer, "<x>%f</x><y>%f</y><z>%f</z>\n", vert.attributes[gdvat].x, vert.attributes[gdvat].y, vert.attributes[gdvat].z );
	fout << buffer;
}

template<GDVertexAttributeType gdvat>
void WriteAttributeBinary(std::ofstream &fout, GDVertex &vert)
{
	fout.write( (const char*)&vert.attributes[gdvat].x, sizeof(vert.attributes[gdvat].x)*3 );
	//char buffer[128];
	//sprintf_s( buffer, "<x>%f</x><y>%f</y><z>%f</z>\n", vert.attributes[gdvat].x, vert.attributes[gdvat].y, vert.attributes[gdvat].z );
	//fout << buffer;
}

template<GDTriangleAttributeType gdtat>
void WriteTriAttributeXML(std::ofstream &fout, GDTriangle &tri)
{
	char buffer[128];
	sprintf_s( buffer, "<x>%f</x><y>%f</y><z>%f</z>\n", tri.attributes[gdtat].normal_x, tri.attributes[gdtat].normal_y, tri.attributes[gdtat].normal_z );
	fout << buffer;
}

template<GDTriangleAttributeType gdtat>
void WriteTriAttributeBinary(std::ofstream &fout, GDTriangle &tri)
{
	fout.write( (const char*)&tri.attributes[gdtat].normal_x, sizeof(tri.attributes[gdtat].normal_x)*3 );
	//char buffer[128];
	//sprintf_s( buffer, "<x>%f</x><y>%f</y><z>%f</z>\n", tri.attributes[gdtat].normal_x, tri.attributes[gdtat].normal_y, tri.attributes[gdtat].normal_z );
	//fout << buffer;
}

template<GDVertexAttributeType gdvat>
int AttributeIndex(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName = 0)
{
	MStatus result;
	int index = polyIt.vertexIndex( polyVertNum, &result );

	if( result == MS::kFailure )
		return -1;

	return index;
}

class GDMeshExportCmd : public MPxCommand
{
	GDMeshObject m_Mesh;

	std::list<GDMeshObject> m_UniqueMeshes;
	std::list<MDagPath> m_ExportedPaths;

	template<GDVertexAttributeType gdvat>
	static void CopyAttributesToVertex(MItMeshPolygon &polyIt, unsigned int polyVertNum, GDVertexAttributeSet* pAttributeSets, GDVertex& out)
	{
		const char* pAttributeName = ( pAttributeSets[gdvat].m_AttributeName.length() == 0 ? 0 : pAttributeSets[gdvat].m_AttributeName.c_str() );
		int attributeIndex = AttributeIndex<gdvat>( polyIt, polyVertNum, pAttributeName );
		if( attributeIndex != -1 && pAttributeSets[gdvat].m_Attributes.size() > (unsigned)attributeIndex )
			out.attributes[gdvat] = pAttributeSets[gdvat].m_Attributes[attributeIndex];

		CopyAttributesToVertex< GDVertexAttributeType(gdvat+1) >( polyIt, polyVertNum, pAttributeSets, out );
	}

	template<>
	static void CopyAttributesToVertex<GDVAT_MAX>(MItMeshPolygon &polyIt, unsigned int polyVertNum, GDVertexAttributeSet* pAttributeSets, GDVertex& out){}

	template<GDTriangleAttributeType gdtat>
	static void CopyAttributesToTriangle(MItMeshPolygon &polyIt, GDTriangleAttributeSet* pAttributeSets, GDTriangle& out, unsigned int polyIndex = 0)
	{
		const char* pAttributeName = ( pAttributeSets[gdtat].m_AttributeName.length() == 0 ? 0 : pAttributeSets[gdtat].m_AttributeName.c_str() );
		
		out.attributes[gdtat] = pAttributeSets[gdtat].m_Attributes[polyIndex];

		CopyAttributesToTriangle< GDTriangleAttributeType(gdtat+1) >( polyIt, pAttributeSets, out, polyIndex );
	}

	template<>
	static void CopyAttributesToTriangle<GDTAT_MAX>(MItMeshPolygon &polyIt, GDTriangleAttributeSet* pAttributeSets, GDTriangle& out, unsigned int polyIndex){}

	template<GDVertexAttributeType gdvat>
	static void CollectAttributes(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
	{
		CollectAttribute<gdvat>( mfnMesh, pAttributeSets );
		CollectAttributes< GDVertexAttributeType(gdvat+1) >( mfnMesh, pAttributeSets );
	}

	template<GDTriangleAttributeType gdtat>
	static void CollectTriAttributes(const MFnMesh& mfnMesh, GDTriangleAttributeSet* pAttributeSets)
	{
		CollectTriAttribute<gdtat>( mfnMesh, pAttributeSets );
		CollectTriAttributes< GDTriangleAttributeType(gdtat+1) >( mfnMesh, pAttributeSets );
	}

	template<>
	static void CollectAttributes<GDVAT_MAX>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets){}

	template<>
	static void CollectTriAttributes<GDTAT_MAX>(const MFnMesh& mfnMesh, GDTriangleAttributeSet* pAttributeSets){}

public:
	GDMeshExportCmd(void){}
	~GDMeshExportCmd()
	{
		Reset();
	}

	void Reset(void);

	// MPxCommand overloads.		
	static void *creator(void);
	static MSyntax newSyntax(void);
	MStatus doIt(const MArgList &args);

	static void CollectAttributes(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
	{
		CollectAttributes< GDVertexAttributeType(GDVAT_INVALID+1) >( mfnMesh, pAttributeSets );
	}

	static void CollectTriAttributes(const MFnMesh& mfnMesh, GDTriangleAttributeSet* pAttributeSets)
	{
		CollectTriAttributes< GDTriangleAttributeType(GDTAT_INVALID+1) >( mfnMesh, pAttributeSets );
	}

	static void CopyAttributesToVertex(MItMeshPolygon &polyIt, unsigned int polyVertNum, GDVertexAttributeSet* pAttributeSets, GDVertex& out)
	{
		CopyAttributesToVertex< GDVertexAttributeType(GDVAT_INVALID+1) >( polyIt, polyVertNum, pAttributeSets, out );
	}

	static void CopyAttributesToTriangle(MItMeshPolygon &polyIt, GDTriangleAttributeSet* pAttributeSets, GDTriangle &out, unsigned int polyIndex)
	{
		CopyAttributesToTriangle< GDTriangleAttributeType(GDTAT_INVALID+1) >( polyIt, pAttributeSets, out, polyIndex );
	}

	static void WriteGDMeshXML(std::ofstream &fout, GDMeshObject &mesh);
	static void WriteGDMeshBinary(std::ofstream &fout, GDMeshObject &mesh);

	template<GDVertexAttributeType gdvat>
	static void WriteAttributeListXML(std::ofstream &fout, GDMeshObject &mesh)
	{
		fout << "<" << GDVertexAttribTypeNames[gdvat] << "_List";
		if( mesh.attributeNames[gdvat].length() == 0 )
			fout << ">\n";
		else
			fout << " name=\"" << mesh.attributeNames[gdvat].c_str() << "\">\n";

		for(unsigned int i = 0; i < mesh.vertices.size(); ++i)
		{
			fout << "<" << GDVertexAttribTypeNames[gdvat] << ">\n";
			WriteAttributeXML<gdvat>(fout, mesh.vertices[i]);
			fout << "</" << GDVertexAttribTypeNames[gdvat] << ">\n";
		}

		fout << "</" << GDVertexAttribTypeNames[gdvat] << "_List>\n";
	}

	template<GDVertexAttributeType gdvat>
	static void WriteAttributeListBinary(std::ofstream &fout, GDMeshObject &mesh)
	{
		unsigned int size = (unsigned int)strlen(GDVertexAttribTypeNames[gdvat]);
		fout.write( (const char*)&size, sizeof(size) );
		fout.write( GDVertexAttribTypeNames[gdvat], size );

		size = (unsigned int)mesh.attributeNames[gdvat].length();
		fout.write( (const char*)&size, sizeof(size) );
		if( size != 0 )
			fout.write( mesh.attributeNames[gdvat].c_str(), size );

		size = AttributeByteCount<gdvat>();
		fout.write( (const char*)&size, sizeof(size) );

		size = (unsigned int)mesh.vertices.size();
		fout.write( (const char*)&size, sizeof(size) );

		for(unsigned int i = 0; i < mesh.vertices.size(); ++i)
			WriteAttributeBinary<gdvat>(fout, mesh.vertices[i]);

		//fout << "<" << GDVertexAttribTypeNames[gdvat] << "_List";
		//if( mesh.attributeNames[gdvat].length() == 0 )
		//	fout << ">\n";
		//else
		//	fout << " name=\"" << mesh.attributeNames[gdvat].c_str() << "\">\n";

		//for(unsigned int i = 0; i < mesh.vertices.size(); ++i)
		//{
		//	fout << "<" << GDVertexAttribTypeNames[gdvat] << ">\n";
		//	WriteAttributeXML<gdvat>(fout, mesh.vertices[i]);
		//	fout << "</" << GDVertexAttribTypeNames[gdvat] << ">\n";
		//}

		//fout << "</" << GDVertexAttribTypeNames[gdvat] << "_List>\n";
	}


	template<GDTriangleAttributeType gdtat>
	static void WriteTriAttributeListXML(std::ofstream &fout, GDMeshObject &mesh)
	{
		fout << "<" << GDTriangleAttribTypeNames[gdtat] << "_List";
		if( mesh.attributeNames[gdtat].length() == 0 )
			fout << ">\n";
		else
			fout << " name=\"" << mesh.triAttributeNames[gdtat].c_str() << "\">\n";

		for(unsigned int i = 0; i < mesh.triangles.size(); ++i)
		{
			fout << "<" << GDTriangleAttribTypeNames[gdtat] << ">\n";
			WriteTriAttributeXML<gdtat>( fout, mesh.triangles[i] );
			fout << "</" << GDTriangleAttribTypeNames[gdtat] << ">\n";
		}

		fout << "</" << GDTriangleAttribTypeNames[gdtat] << "_List>\n";
	}

	template<GDTriangleAttributeType gdtat>
	static void WriteTriAttributeListBinary(std::ofstream &fout, GDMeshObject &mesh)
	{
		unsigned int size = (unsigned int)strlen(GDTriangleAttribTypeNames[gdtat]);
		fout.write( (const char*)&size, sizeof(size) );
		fout.write( GDTriangleAttribTypeNames[gdtat], size );

		size = (unsigned int)mesh.triAttributeNames[gdtat].length();
		fout.write( (const char*)&size, sizeof(size) );
		if( size != 0 )
			fout.write( mesh.triAttributeNames[gdtat].c_str(), size );
		
		size = TriAttributeByteCount<gdtat>();
		fout.write( (const char*)&size, sizeof(size) );

		size = (unsigned int)mesh.triangles.size();
		fout.write( (const char*)&size, sizeof(size) );

		for(unsigned int i = 0; i < mesh.triangles.size(); ++i)
			WriteTriAttributeBinary<gdtat>( fout, mesh.triangles[i] );

		//fout << "<" << GDTriangleAttribTypeNames[gdtat] << "_List";
		//if( mesh.attributeNames[gdtat].length() == 0 )
		//	fout << ">\n";
		//else
		//	fout << " name=\"" << mesh.triAttributeNames[gdtat].c_str() << "\">\n";

		//for(unsigned int i = 0; i < mesh.triangles.size(); ++i)
		//{
		//	fout << "<" << GDTriangleAttribTypeNames[gdtat] << ">\n";
		//	WriteTriAttributeXML<gdtat>( fout, mesh.triangles[i] );
		//	fout << "</" << GDTriangleAttribTypeNames[gdtat] << ">\n";
		//}

		//fout << "</" << GDTriangleAttribTypeNames[gdtat] << "_List>\n";
	}

	template<GDVertexAttributeType gdvat>
	static void WriteAttributeListsXML(std::ofstream &fout, GDMeshObject &mesh)
	{
		if( mesh.attributePresent[gdvat] )
		{	
			fout << "<GDMeshAttribute>\n";
			WriteAttributeListXML<gdvat>(fout, mesh);
			fout << "</GDMeshAttribute>\n";
		}

		WriteAttributeListsXML< GDVertexAttributeType(gdvat+1) >(fout, mesh);
	}

	template<>
	static void WriteAttributeListsXML<GDVAT_MAX>(std::ofstream &fout, GDMeshObject &mesh)
	{
	}

	template<GDVertexAttributeType gdvat>
	static void WriteAttributeListsBinary(std::ofstream &fout, GDMeshObject &mesh)
	{
		if( mesh.attributePresent[gdvat] )
		{	
			WriteAttributeListBinary<gdvat>(fout, mesh);
			//fout << "<GDMeshAttribute>\n";
			//WriteAttributeListXML<gdvat>(fout, mesh);
			//fout << "</GDMeshAttribute>\n";
		}

		WriteAttributeListsBinary< GDVertexAttributeType(gdvat+1) >(fout, mesh);
	}

	template<>
	static void WriteAttributeListsBinary<GDVAT_MAX>(std::ofstream &fout, GDMeshObject &mesh)
	{
	}


	template<GDTriangleAttributeType gdtat>
	static void WriteTriAttributeListsXML(std::ofstream &fout, GDMeshObject &mesh)
	{
		fout << "<GDMeshAttribute>\n";
		WriteTriAttributeListXML<gdtat>(fout, mesh);
		fout << "</GDMeshAttribute>\n";

		WriteTriAttributeListsXML< GDTriangleAttributeType(gdtat+1) >(fout, mesh);
	}

	template<>
	static void WriteTriAttributeListsXML<GDTAT_MAX>(std::ofstream &fout, GDMeshObject &mesh)
	{
	}

	template<GDTriangleAttributeType gdtat>
	static void WriteTriAttributeListsBinary(std::ofstream &fout, GDMeshObject &mesh)
	{
		//fout << "<GDMeshAttribute>\n";
		//WriteTriAttributeListXML<gdtat>(fout, mesh);
		//fout << "</GDMeshAttribute>\n";
		WriteTriAttributeListBinary<gdtat>(fout, mesh);

		WriteTriAttributeListsBinary< GDTriangleAttributeType(gdtat+1) >(fout, mesh);
	}

	template<>
	static void WriteTriAttributeListsBinary<GDTAT_MAX>(std::ofstream &fout, GDMeshObject &mesh)
	{
	}


	void GetUniqueMeshes(MFnTransform &transform);

	static void ExportMesh(MFnMesh& currMesh, GDMeshObject& outMesh);

	static void AddRenderContextAttribute(MFnMesh& mesh);
};


