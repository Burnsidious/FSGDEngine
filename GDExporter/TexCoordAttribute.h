#pragma once

#include "GDMeshExportCmd.h"

//template<>
//void GDMeshExportCmd::CollectAttribute<GDVAT_TEXCOORDS_0>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
//{
//	CollectTexCoordAttribute( mfnMesh, pAttributeSets, 0 );
//}

template<>
int AttributeIndex<GDVAT_TEXCOORDS_0>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName);

//template<>
//void GDMeshExportCmd::CollectAttribute<GDVAT_TEXCOORDS_1>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
//{
//	CollectTexCoordAttribute( mfnMesh, pAttributeSets, 1 );
//}

template<>
int AttributeIndex<GDVAT_TEXCOORDS_1>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName);


//template<>
//void GDMeshExportCmd::CollectAttribute<GDVAT_TEXCOORDS_2>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
//{
//	CollectTexCoordAttribute( mfnMesh, pAttributeSets, 2 );
//}

template<>
int AttributeIndex<GDVAT_TEXCOORDS_2>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName);

template<>
void CollectAttribute<GDVAT_TEXCOORDS_0>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets);

template<>
void CollectAttribute<GDVAT_TEXCOORDS_1>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets);

template<>
void CollectAttribute<GDVAT_TEXCOORDS_2>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets);

template<>
void WriteAttributeXML<GDVAT_TEXCOORDS_0>(std::ofstream &fout, GDVertex &vert);

template<>
void WriteAttributeXML<GDVAT_TEXCOORDS_1>(std::ofstream &fout, GDVertex &vert);

template<>
void WriteAttributeXML<GDVAT_TEXCOORDS_2>(std::ofstream &fout, GDVertex &vert);

template<>
void WriteAttributeBinary<GDVAT_TEXCOORDS_0>(std::ofstream &fout, GDVertex &vert);

template<>
void WriteAttributeBinary<GDVAT_TEXCOORDS_1>(std::ofstream &fout, GDVertex &vert);

template<>
void WriteAttributeBinary<GDVAT_TEXCOORDS_2>(std::ofstream &fout, GDVertex &vert);

template<>
unsigned int AttributeByteCount<GDVAT_TEXCOORDS_0>(void);

template<>
unsigned int AttributeByteCount<GDVAT_TEXCOORDS_1>(void);

template<>
unsigned int AttributeByteCount<GDVAT_TEXCOORDS_2>(void);