#ifndef DEPTH_SPOT_VS_HLSL
#define DEPTH_SPOT_VS_HLSL

#include "../EDRendererD3D/ConstantBuffers.h"
#include "VertexLayouts.hlsli"
#include "ShadowSupport.hlsli"
#include "../EDRendererD3D/LightBuffers.h"

DepthVertOut main(VERTEX_POSNORMTEX input)
{
	DepthVertOut outVert;
	
	outVert.position = mul(float4(input.position, 1.0), gWorld);
	float3 norm = mul(float4(input.normal, 0), gWorld).xyz;
	outVert.position.xyz -= norm * ShadowNormOffset;

	outVert.position = mul(outVert.position, spotLightShadowData.viewProjection);

    outVert.depth.xy = outVert.position.zw;

	outVert.texCoord = input.texCoord;

	return outVert;
}

#endif //DEPTH_SPOT_VS_HLSL