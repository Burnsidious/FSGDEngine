#pragma once

#include "GDMeshExportCmd.h"

template<>
void CollectAttribute<GDVAT_JOINTS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets);

template<>
void WriteAttributeXML<GDVAT_JOINTS>(std::ofstream &fout, GDVertex &vert);

template<>
void WriteAttributeXML<GDVAT_WEIGHTS>(std::ofstream &fout, GDVertex &vert);

template<>
void WriteAttributeBinary<GDVAT_JOINTS>(std::ofstream &fout, GDVertex &vert);

template<>
void WriteAttributeBinary<GDVAT_WEIGHTS>(std::ofstream &fout, GDVertex &vert);

template<>
unsigned int AttributeByteCount<GDVAT_JOINTS>(void);

template<>
unsigned int AttributeByteCount<GDVAT_WEIGHTS>(void);

