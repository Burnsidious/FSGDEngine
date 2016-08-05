#pragma once

#include "GDMeshExportCmd.h"

template<>
void CollectAttribute<GDVAT_POINTS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets);