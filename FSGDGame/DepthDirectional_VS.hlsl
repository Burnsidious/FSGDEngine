#ifndef DEPTH_DIRECTIONAL_VS_HLSL
#define DEPTH_DIRECTIONAL_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/LightBuffers.h"
#include "../EDRendererD3D/ConstantBuffers.h"
#include "ShadowSupport.hlsli"

DepthVertOut main(VERTEX_POSNORMTEX input)
{
	DepthVertOut output;
	float3 norm;

	output.position = mul(float4(input.position, 1.0), gWorld);
	norm = mul(float4(input.normal, 0), gWorld).xyz;
	output.position.xyz -= norm * ShadowNormOffset;
	output.position = mul(output.position, DirLight.viewProjection[DirLight.currentIndex]);

	output.texCoord = input.texCoord;
	output.position.z = max(0, output.position.z);
	output.depth.xy = output.position.zw;

	return output;
}

#endif //DEPTH_DIRECTIONAL_VS_HLSL