#ifndef TESSELATION_HLSLI
#define TESSELATION_HLSLI

#include "../EDRendererD3D/ConstantBuffers.h"

float DistanceBasedTesselation(float3 positonW)
{
	//return 15;
	float dist = distance(gCameraPos, positonW);

	float tessScale = saturate((gMaxTessDistance - dist) / (gMaxTessDistance - gMinTessDistance) );
	return gMinTessFactor + tessScale * (gMaxTessFactor - gMinTessFactor);
}

#endif //TESSELATION_HLSLI