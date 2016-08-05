#pragma once

#include "GDMeshExportCmd.h"

template<>
void CollectAttribute<GDVAT_NORMALS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets);

template<>
int AttributeIndex<GDVAT_NORMALS>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName);