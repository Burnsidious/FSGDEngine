#pragma once

#include "float3.h"

namespace EDMath
{
	void clipPolygonToPlaneNegative(const Float3& planeN, float planeOffset, const Float3* verts, unsigned int vertCount, Float3* out, unsigned int& outCount);
}