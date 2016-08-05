#pragma once

#include "GDMeshExportCmd.h"

template<>
void CollectTriAttribute<GDTAT_TRINEIGHBORS>(const MFnMesh& mfnMesh, GDTriangleAttributeSet* pAttributeSets);

template<>
void WriteTriAttributeXML<GDTAT_TRINEIGHBORS>(std::ofstream &fout, GDTriangle &tri);

template<>
void WriteTriAttributeBinary<GDTAT_TRINEIGHBORS>(std::ofstream &fout, GDTriangle &tri);