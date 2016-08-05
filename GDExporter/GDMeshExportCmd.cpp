#include "GDMeshExportCmd.h"
#include "GDAttribute.h"
#include "GDExporter.h"

#include <maya/MSelectionList.h>
#include <maya/MSyntax.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

//#include "../EDMath/Float3.h"
//using namespace EDUtilities;

#include "PointAttribute.h"
#include "NormalAttribute.h"
#include "TangentAttribute.h"
#include "BitangentAttribute.h"
#include "TexCoordAttribute.h"
#include "InfluenceAttribute.h"
#include "TriNormalAttribute.h"
#include "TriNeighborAttribute.h"
#include "../EDMath/Float3.h"

using namespace std;
//#include "GDMeshBVH.h"

//#include "../EDTree/AabbBvh.h"

//bool GDMeshExportCmd::m_ExportBVH = false;

//const char *bvhFlag = "-b", *bvhLongFlag = "-bvhierarchy";

void *GDMeshExportCmd::creator(void)
{
	return new GDMeshExportCmd;
}

MSyntax GDMeshExportCmd::newSyntax(void)
{
	MSyntax syntax;

	//syntax.addFlag( bvhFlag, bvhLongFlag, MSyntax::kBoolean );

	return syntax;
}

MStatus GDMeshExportCmd::doIt(const MArgList &args)
{
	Reset();

	MArgDatabase arguments(syntax(), args);

	//if( arguments.isFlagSet( bvhFlag ) )
	//	arguments.getFlagArgument( bvhFlag, 0 , m_ExportBVH );

	const char* exportPath = GDExporter::GetInstance()->EstablishExportPath();

	if( exportPath == 0 )
	{
		MGlobal::displayError(MString("GDMeshExport - No export path established."));
		return MS::kFailure;
	}

	MDagPath meshTransformPath;
	if( !GDExporter::GetInstance()->GetSelectedMeshTransformPath( meshTransformPath ) )
	{
		MGlobal::displayError("GDMeshExport - No meshes selected.");
		return MS::kFailure;
		//MFnTransform selectedTransform(meshTransformPath);
		//GetUniqueMeshes(selectedTransform);
	}
	
	MFnTransform selectedTransform(meshTransformPath);
	GetUniqueMeshes(selectedTransform);

	std::list<GDMeshObject>::iterator uniqueIter = m_UniqueMeshes.begin();
	for(; uniqueIter != m_UniqueMeshes.end(); ++uniqueIter)
	{
		//MStringArray pathStepNames;
		//MString( (*uniqueIter).meshName.c_str() ).split( '|', pathStepNames );

		//std::string filePath = exportPath;
		//filePath += "/GDMesh";

		//_mkdir( filePath.c_str() );

		//filePath += "/";
		//
		//std::string sceneName;
		//GDExporter::GetInstance()->GetSceneName(sceneName);
		//filePath += sceneName;

		//_mkdir( filePath.c_str() );

		//if( pathStepNames.length() > 1 )
		//{
		//	for(unsigned int i = 0; i < pathStepNames.length()-1; ++i)
		//	{
		//		filePath += "/";
		//		filePath += pathStepNames[i].asChar();
		//		_mkdir( filePath.c_str() );
		//	}
		//}

		//filePath += "/";
		//filePath += pathStepNames[ pathStepNames.length()-1 ].asChar();
		//filePath += ".xml";

		//if( !GDExporter::GetInstance()->ValidateFileCreation( filePath.c_str() ) )
		//{
		//	continue;
		//}

		if( !GDExporter::GetInstance()->ValidateFileCreation( (*uniqueIter).meshFilePath.c_str() ) )
		{
			MGlobal::displayInfo( MString((*uniqueIter).meshName.c_str()) + " export canceled." );
			continue;
		}

		string formattedMeshPath = (*uniqueIter).meshFilePath.c_str();
		
		std::ofstream fout( formattedMeshPath, std::ios_base::out | ios_base::binary | ios_base::trunc );
		WriteGDMeshXML( fout, (*uniqueIter) );
		fout.close();

		size_t formattedMeshPathLength = strlen(formattedMeshPath.c_str()) +1;
		char *binBuffer = new char[formattedMeshPathLength];
		strcpy_s( binBuffer, formattedMeshPathLength, formattedMeshPath.c_str() );
		size_t len = formattedMeshPath.length();
		binBuffer[ len - 3 ] = 'b';
		binBuffer[ len - 2 ] = 'i';
		binBuffer[ len - 1 ] = 'n';

		std::ofstream fout2( binBuffer, ios_base::out | ios_base::trunc | ios_base::binary );
		delete [] binBuffer;
		WriteGDMeshBinary( fout2, (*uniqueIter ) );
		fout2.close();
	}

	return MS::kSuccess;

	//MFnMesh* exportingMesh = GDExporter::GetInstance()->GetSelectedMesh();

	//if( exportingMesh == 0 )
	//	return MS::kFailure;

	//MStringArray pathStepNames;
	//exportingMesh->fullPathName().split('|', pathStepNames);

	//std::string filePath = exportPath;
	//std::string relativePath = "/GDMesh";
	//filePath += "/GDMesh";
	//_mkdir( filePath.c_str() );

	//for(unsigned int i = 0; i < pathStepNames.length(); ++i)
	//{
	//	relativePath += "/";
	//	relativePath += pathStepNames[i].asChar();
	//	filePath += "/";
	//	filePath += pathStepNames[i].asChar();
	//	_mkdir( filePath.c_str() );		
	//}
	//
	//std::string fileName = "msh_";
	//fileName += exportingMesh->name().asChar();
	//fileName += ".xml";

	//filePath += "/";
	//filePath += fileName;

	//relativePath += "/";
	//relativePath += fileName;

	//if( !GDExporter::GetInstance()->ValidateFileCreation( filePath.c_str() ) )
	//{
	//	MGlobal::displayInfo( exportingMesh->partialPathName() + " export canceled." );
	//	delete exportingMesh;
	//	return MS::kFailure;
	//}
	//
	//GDVertexAttributeSet meshVertexAttributes[GDVAT_MAX];

	//CollectAttributes( *exportingMesh, meshVertexAttributes );

	//MDagPath meshPath;
	//exportingMesh->getPath(meshPath);
	//
	//MIntArray triangleCounts;
	//MIntArray triangleVertices;
	//exportingMesh->getTriangles( triangleCounts, triangleVertices );

	//std::vector<int> vecTriangleCounts; 
	//std::vector<int> vecTriangleVertices; 
	//
	//vecTriangleCounts.resize( triangleCounts.length() );
	//vecTriangleVertices.resize( triangleVertices.length() );

	//for(unsigned int i =0; i < triangleCounts.length(); ++i)
	//	vecTriangleCounts[i] = triangleCounts[i];

	//for(unsigned int i =0; i < triangleVertices.length(); ++i)
	//	vecTriangleVertices[i] = triangleVertices[i];

	//MItMeshPolygon polyIter(meshPath); 

	//unsigned int polyIndex = 0;
	//unsigned int processedTriCount = 0;
	//for(; !polyIter.isDone(); polyIter.next(), ++polyIndex )
	//{
	//	std::vector<int> localIndices;
	//	localIndices.reserve( vecTriangleCounts[polyIndex] * 3 );

	//	for(int i = 0; i < vecTriangleCounts[polyIndex]; ++i)
	//	{
	//		int triIndices[3];
	//		for(unsigned int j =0; j < 3; ++j)
	//		{
	//			int vertIndex = vecTriangleVertices[ (processedTriCount + i) * 3 + j ];
	//			unsigned int k = 0;
	//			for(; k < localIndices.size(); ++k)
	//				if( localIndices[k] == vertIndex )
	//					break;

	//			if( k == localIndices.size() )
	//				localIndices.push_back( vertIndex );

	//			triIndices[j] = k;
	//		}

	//		for(unsigned int j = 0; j < 3; ++j)
	//		{
	//			GDVertex tempVert;
	//			CopyAttributesToVertex(polyIter, triIndices[j], meshVertexAttributes, tempVert);

	//			unsigned int vertCount = m_Mesh.vertices.size();
	//			unsigned int v = 0;
	//			for(; v < vertCount; ++v)
	//				if( m_Mesh.vertices[v] == tempVert )
	//					break;
	//				
	//			if( v == vertCount )
	//				m_Mesh.vertices.push_back(tempVert);

	//			m_mesh.indices.push_back( v );
	//		}
	//	}

	//	processedTriCount += vecTriangleCounts[polyIndex];
	//	//for(unsigned int i = 0; i < 3; ++i)
	//	//{
	//	//	GDVertex tempVert;
	//	//	CopyAttributesToVertex(polyIter, i, meshVertexAttributes, tempVert);

	//	//	unsigned int vertCount = m_Mesh.vertices.size();
	//	//	unsigned int v = 0;
	//	//	for(; v < vertCount; ++v)
	//	//		if( m_Mesh.vertices[v] == tempVert )
	//	//			break;
	//	//	
	//	//	if( v == vertCount )
	//	//		m_Mesh.vertices.push_back(tempVert);

	//	//	indices.push_back( v );
	//	//}
	//}

	//m_Mesh.meshName = exportingMesh->name().asChar();

	//for(unsigned int i = 0; i < GDVAT_MAX; ++i)
	//{
	//	m_Mesh.triAttributeNames[i] = meshVertexAttributes[i].m_AttributeName;
	//	m_Mesh.triAttributePresent[i] = (meshVertexAttributes[i].attributes.size() != 0);
	//}

	//ofstream fout( filePath.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );

	//WriteGDMeshXML(fout, m_Mesh);

	//fout.close();

	//MFnDependencyNode depNode(meshPath.node());
	//MStatus fpResult;
	//MPlug gdMeshPlug = exportingMesh->findPlug("GDMesh", &fpResult);
	//
	//if( fpResult != MS::kSuccess )
	//{
	//	MStatus result;
	//	MFnTypedAttribute typedAttr;
	//	MObject newAttribute= typedAttr.create( "GDMesh", "GDMsh", MFnData::kString, &result);
	//	typedAttr.setHidden(false);
	//	exportingMesh->addAttribute( newAttribute );
	//}

	//gdMeshPlug = exportingMesh->findPlug("GDMesh", &fpResult);
	//if( fpResult == MS::kSuccess )
	//	gdMeshPlug.setValue(relativePath.c_str());

	//delete exportingMesh;
	//return MS::kSuccess;
}

void GDMeshExportCmd::Reset(void)
{
	m_Mesh.vertices.clear();
	m_Mesh.meshName.clear();
	m_Mesh.indices.clear();

	memset( m_Mesh.triAttributePresent, 0, sizeof(bool)*GDTAT_MAX );
	for(unsigned int i = 0; i < GDTAT_MAX; ++i)
		m_Mesh.triAttributeNames[i].clear();

	m_UniqueMeshes.clear();

	m_ExportedPaths.clear();
}

void GDMeshExportCmd::ExportMesh(MFnMesh& currMesh, GDMeshObject& outMesh)
{
	GDVertexAttributeSet meshVertexAttributes[GDVAT_MAX];
	GDTriangleAttributeSet meshTriangleAttributes[GDTAT_MAX];

	CollectAttributes( currMesh, meshVertexAttributes );
	CollectTriAttributes( currMesh, meshTriangleAttributes );

	MDagPath meshPath;
	currMesh.getPath(meshPath);
	
	MIntArray triangleCounts;
	//MIntArray triangleVertices;
	//currMesh.getTriangles( triangleCounts, triangleVertices );

	//// These variables do nothing?  Seems like it.
	//// They get filled, and then... nothing?
	//std::vector<int> vecTriangleCounts; 
	//std::vector<int> vecTriangleVertices; 
	//
	//vecTriangleCounts.resize( triangleCounts.length() );
	//vecTriangleVertices.resize( triangleVertices.length() );

	//for(unsigned int i =0; i < triangleCounts.length(); ++i)
	//	vecTriangleCounts[i] = triangleCounts[i];

	//for(unsigned int i =0; i < triangleVertices.length(); ++i)
	//	vecTriangleVertices[i] = triangleVertices[i];

	MItMeshPolygon polyIter(meshPath); 

	unsigned int polyIndex = 0;
	unsigned int processedTriCount = 0;
	for(; !polyIter.isDone(); polyIter.next(), ++polyIndex )
	{
		//std::vector<int> localIndices;
		//localIndices.reserve( vecTriangleCounts[polyIndex] * 3 );

		//for(int i = 0; i < vecTriangleCounts[polyIndex]; ++i)
		//{
		//	int triIndices[3];
		//	for(unsigned int j =0; j < 3; ++j)
		//	{
		//		int vertIndex = vecTriangleVertices[ (processedTriCount + i) * 3 + j ];
		//		unsigned int k = 0;
		//		for(; k < localIndices.size(); ++k)
		//			if( localIndices[k] == vertIndex )
		//				break;

		//		if( k == localIndices.size() )
		//			localIndices.push_back( vertIndex );

		//		triIndices[j] = k;
		//	}

		//	for(unsigned int j = 0; j < 3; ++j)
		//	{
		//		GDVertex tempVert;
		//		CopyAttributesToVertex(polyIter, triIndices[j], meshVertexAttributes, tempVert);

		//		unsigned int vertCount = outMesh.vertices.size();
		//		unsigned int v = 0;
		//		for(; v < vertCount; ++v)
		//			if( outMesh.vertices[v] == tempVert )
		//				break;
		//			
		//		if( v == vertCount )
		//			outMesh.vertices.push_back(tempVert);

		//		outmesh.indices.push_back( v );
		//	}
		//}

		//processedTriCount += vecTriangleCounts[polyIndex];
		
		for(unsigned int i = 0; i < 3; ++i)
		{
			GDVertex tempVert;
			CopyAttributesToVertex(polyIter, i, meshVertexAttributes, tempVert);

			size_t vertCount = outMesh.vertices.size();
			size_t v = 0;
			for(; v < vertCount; ++v)
				if( outMesh.vertices[v] == tempVert )
					break;
			
			if( v == vertCount )
				outMesh.vertices.push_back(tempVert);

			outMesh.indices.push_back( static_cast<unsigned int>(v) );
		}

		// Reverse the winding order.
		// Convert to left-handed coordinate system.
		unsigned int temp = outMesh.indices[outMesh.indices.size() - 2];
		outMesh.indices[outMesh.indices.size() - 2] = outMesh.indices[outMesh.indices.size() - 1];
		outMesh.indices[outMesh.indices.size() - 1] = temp;

		outMesh.triangles.resize( outMesh.triangles.size() + 1 );
		CopyAttributesToTriangle( polyIter, meshTriangleAttributes, (*outMesh.triangles.rbegin()), polyIndex );
	} 

	outMesh.meshName = currMesh.partialPathName().asChar();

	for(unsigned int i = 0; i < GDVAT_MAX; ++i)
	{
		outMesh.attributeNames[i] = meshVertexAttributes[i].m_AttributeName;
		outMesh.attributePresent[i] = (meshVertexAttributes[i].m_Attributes.size() != 0);
	}

	for(unsigned int i = 0; i < outMesh.indices.size(); ++i)
	{
		GDVertex& vert = outMesh.vertices[ outMesh.indices[i] ];
		GDVertexAttribute& tangent = vert.attributes[GDVAT_TANGENTS];
		GDVertexAttribute& bitangent = vert.attributes[GDVAT_BITANGENTS];
		GDVertexAttribute& normal = vert.attributes[GDVAT_NORMALS];

		EDMath::Float3 x( tangent.x, tangent.y, tangent.z );
		EDMath::Float3 y( bitangent.x, bitangent.y, bitangent.z );
		EDMath::Float3 z( normal.x, normal.y, normal.z );
		EDMath::Float3 perp;
		CrossProduct( perp, y, z );
		tangent.w = DotProduct( perp, x );
	}

	for(unsigned int i = 0; i < GDTAT_MAX; ++i)
	{
		outMesh.triAttributeNames[i] = meshTriangleAttributes[i].m_AttributeName;
		outMesh.triAttributePresent[i] = (meshTriangleAttributes[i].m_Attributes.size() != 0);
	}
}

void GDMeshExportCmd::GetUniqueMeshes(MFnTransform &transform)
{
	const char* exportPath = GDExporter::GetInstance()->EstablishExportPath();

	if( exportPath == 0 )
		return;

	for(unsigned int i = 0; i < transform.childCount(); ++i)
	{
		if( transform.child(i).apiType() == MFn::kMesh )
		{
			MFnMesh childMesh( transform.child(i) );

			if( childMesh.isIntermediateObject() )
				continue;

			MDagPath meshPath;
			childMesh.getPath(meshPath);
			
			if( childMesh.isInstanced(false) )
			{
				std::list<MDagPath>::iterator iter = m_ExportedPaths.begin();
				bool bExported = false;
				for(; iter != m_ExportedPaths.end(); ++iter)
				{
					if( (*iter) == meshPath )
					{
						bExported = true;
						break;
					}
				}

				if( bExported == true )
					continue;
			}

			m_ExportedPaths.push_back( meshPath );

			m_UniqueMeshes.push_back(GDMeshObject());
			GDMeshObject& exportingMesh = (*(m_UniqueMeshes.rbegin()));
			ExportMesh(childMesh, exportingMesh );

			MStringArray pathStepNames;
			MString( exportingMesh.meshName.c_str() ).split( '|', pathStepNames );

			std::string filePath = exportPath;
			std::string relativePath = "/GDMesh";
			filePath += "/GDMesh";

			_mkdir( filePath.c_str() );

			relativePath += "/";
			filePath += "/";
		
			std::string sceneName;
			GDExporter::GetInstance()->GetSceneName(sceneName);
			relativePath += sceneName;	
			filePath += sceneName;

			_mkdir( filePath.c_str() );

			if( pathStepNames.length() > 1 )
			{
				for(unsigned int i = 0; i < pathStepNames.length()-1; ++i)
				{
					relativePath += "/";
					relativePath += pathStepNames[i].asChar();
					filePath += "/";
					filePath += pathStepNames[i].asChar();
					_mkdir( filePath.c_str() );
				}
			}

			relativePath += "/";
			relativePath += pathStepNames[ pathStepNames.length()-1 ].asChar();
			relativePath += ".xml";

			filePath += "/";
			filePath += pathStepNames[ pathStepNames.length()-1 ].asChar();
			filePath += ".xml";
			
			exportingMesh.meshFilePath = filePath;

			// We need to replace the ":"s that Maya will add to file names, but not replace the :
			// after a drive letter like C:/
			string::size_type loc = exportingMesh.meshFilePath.find(":");
			loc = exportingMesh.meshFilePath.find(":", loc+1);

			while (loc != string::npos)
			{
				exportingMesh.meshFilePath.replace(loc, 1, "-");
				loc = exportingMesh.meshFilePath.find(":", loc+1);
			} 

			MFnDependencyNode depNode(meshPath.node());
			MStatus fpResult;
			MPlug gdMeshPlug = childMesh.findPlug("GDMesh", &fpResult);
			
			if( fpResult != MS::kSuccess )
			{
				MStatus result;
				MFnTypedAttribute typedAttr;
				MObject newAttribute= typedAttr.create( "GDMesh", "GDMsh", MFnData::kString, &result);
				typedAttr.setHidden(false);
				childMesh.addAttribute( newAttribute );
			}

			gdMeshPlug = childMesh.findPlug("GDMesh", &fpResult);
			if( fpResult == MS::kSuccess )
				gdMeshPlug.setValue(relativePath.c_str());

			AddRenderContextAttribute(childMesh);
		}
		else if( transform.child(i).apiType() == MFn::kTransform )
		{
			MFnTransform childTransform( transform.child(i) );
			GetUniqueMeshes( childTransform );			
		}
	}
}

void CollectTexCoordAttribute( const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets, unsigned int setNumber )
{
	MStringArray setNames;
	mfnMesh.getUVSetNames( setNames );

	if( setNames.length() < setNumber+1 )
		return;

	pAttributeSets[GDVAT_TEXCOORDS_0 + setNumber].m_AttributeName = setNames[setNumber].asChar();
	
	MFloatArray uArray;
	MFloatArray vArray;

	mfnMesh.getUVs( uArray, vArray, &setNames[setNumber] );

	pAttributeSets[GDVAT_TEXCOORDS_0 + setNumber].m_Attributes.resize( uArray.length() );

	for(unsigned int i = 0; i < uArray.length(); ++i)
	{
		pAttributeSets[GDVAT_TEXCOORDS_0 + setNumber].m_Attributes[i].s = uArray[i];
		pAttributeSets[GDVAT_TEXCOORDS_0 + setNumber].m_Attributes[i].t = vArray[i];
	}
}

void GDMeshExportCmd::WriteGDMeshBinary(std::ofstream &fout, GDMeshObject &mesh)
{
	unsigned int size = static_cast<unsigned int>(mesh.meshName.length());
	fout.write( (const char*)&size, sizeof(size) );
	fout.write( mesh.meshName.c_str(), size );

	size = (unsigned int)mesh.vertices.size();
	fout.write( (const char*)&size, sizeof(size) );
	
	size = (unsigned int)mesh.indices.size()/3;
	fout.write( (const char*)&size, sizeof(size) );
	
	for(unsigned int i = 0; i < size; ++i)
	{
		fout.write( (const char*)&mesh.indices[i*3], sizeof(int) );
		fout.write( (const char*)&mesh.indices[i*3+1], sizeof(int) );
		fout.write( (const char*)&mesh.indices[i*3+2], sizeof(int) );
	}

	unsigned int attrCount = 0;
	for(unsigned int i = 0; i < GDTAT_MAX; ++i)
	{
		if( mesh.triAttributePresent[i] )
			attrCount += 1;
	}
	
	fout.write( (const char*)&attrCount, sizeof(unsigned int) );

	WriteAttributeListsBinary< GDVertexAttributeType( GDVAT_INVALID + 1 ) >( fout, mesh );
	WriteTriAttributeListsBinary<GDTriangleAttributeType( GDTAT_INVALID + 1 ) >( fout, mesh );
}

void GDMeshExportCmd::WriteGDMeshXML(std::ofstream &fout, GDMeshObject &mesh)
{
	char buffer[128];
	fout << "<?xml version=\"1.0\"?>\n";
	fout << "<GDMesh name=\"" << mesh.meshName.c_str() << "\">\n";
	sprintf_s(buffer, "%d", mesh.vertices.size());
	fout << "\t<NumVerts>" << buffer << "</NumVerts>\n";

	size_t triCount = mesh.indices.size()/3;

	sprintf_s(buffer, "\t<Triangle_List count=\"%d\">", triCount );
	fout << buffer;

	for(size_t i = 0; i < triCount; ++i)
	{
		sprintf_s(buffer, "\t\t<Triangle n=\"%d\">\n", i );
		fout << buffer;
		sprintf_s(buffer, "\t\t\t<Index n=\"%d\">", 0 );
		fout << buffer;
		sprintf_s(buffer, "%d</Index>\n", mesh.indices[i*3]);
		fout << buffer;
		sprintf_s(buffer, "\t\t\t<Index n=\"%d\">", 1 );
		fout << buffer;
		sprintf_s(buffer, "%d</Index>\n", mesh.indices[i*3+1]);
		fout << buffer;
		sprintf_s(buffer, "\t\t\t<Index n=\"%d\">", 2 );
		fout << buffer;
		sprintf_s(buffer, "%d</Index>\n", mesh.indices[i*3+2]);
		fout << buffer;
		fout << "\t\t</Triangle>\n";
	}

	fout << "\t</Triangle_List>\n";

	unsigned int attrCount = 0;
	for(unsigned int i = 0; i < GDTAT_MAX; ++i)
	{
		if( mesh.triAttributePresent[i] )
			attrCount += 1;
	}

	sprintf_s(buffer, "\t<GDMeshAttribute_List count=\"%d\">\n", attrCount);
	fout << buffer;
	
	WriteAttributeListsXML<GDVAT_POINTS>( fout, mesh );
	WriteTriAttributeListsXML< GDTriangleAttributeType( GDTAT_INVALID + 1 ) >( fout, mesh );

	fout << "\t</GDMeshAttribute_List>\n";


	fout << "</GDMesh>\n";		
}



void GDMeshExportCmd::AddRenderContextAttribute(MFnMesh& mesh)
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
		namePlug.setValue("LitTexturedShadows");

		MPlug fxPlug = rcPlug.child(2);
		fxPlug.setValue("LitTextured.fx");
		
		MPlug techPlug = rcPlug.child(3);
		techPlug.setValue("Basic");
	}
}












