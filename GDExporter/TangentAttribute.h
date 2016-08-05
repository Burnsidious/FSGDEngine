#pragma once

#include "GDMeshExportCmd.h"

//template<>
//void GDMeshExportCmd::CollectAttribute<GDVAT_TANGENTS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
//{
//	MFloatVectorArray tangents;
//	mfnMesh.getTangents( tangents );
//
//	pAttributeSets[GDVAT_TANGENTS].attributes.resize( tangents.length() );
//
//	for(unsigned int i = 0; i < tangents.length(); ++i)
//	{
//		pAttributeSets[GDVAT_TANGENTS].attributes[i].x = tangents[i].x;
//		pAttributeSets[GDVAT_TANGENTS].attributes[i].y = tangents[i].y;
//		pAttributeSets[GDVAT_TANGENTS].attributes[i].z = tangents[i].z;
//	}
//}

template<>
int AttributeIndex<GDVAT_TANGENTS>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName);

template<>
void CollectAttribute<GDVAT_TANGENTS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets);

template<>
void WriteAttributeXML<GDVAT_TANGENTS>(std::ofstream &fout, GDVertex &vert);

template<>
void WriteAttributeBinary<GDVAT_TANGENTS>(std::ofstream &fout, GDVertex &vert);

template<>
unsigned int AttributeByteCount<GDVAT_TANGENTS>(void);
