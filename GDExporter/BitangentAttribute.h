#pragma once

#include "GDMeshExportCmd.h"

template<>
void CollectAttribute<GDVAT_BITANGENTS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets);

template<>
int AttributeIndex<GDVAT_BITANGENTS>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName);