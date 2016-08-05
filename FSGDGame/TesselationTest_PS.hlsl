#ifndef TESSELATION_TEST_PS_HLSL
#define TESSELATION_TEST_PS_HLSL

#include "VertexLayouts.hlsli"
#include "ForwardlightingSupport.hlsli"

#include "../EDRendererD3D/LightBuffers.h"
#include "../EDRendererD3D/ConstantBuffers.h"

float4 main(TESSELATION_TEST_HS_OUT input) : SV_TARGET
{
	return diffuseMap.Sample(anisoWrapSampler, input.texCoord);
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

#endif //TESSELATION_TEST_PS_HLSL