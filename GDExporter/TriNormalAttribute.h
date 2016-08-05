#pragma once

#include "GDMeshExportCmd.h"

template<>
void CollectTriAttribute<GDTAT_TRINORMS>(const MFnMesh& mfnMesh, GDTriangleAttributeSet* pAttributeSets);

