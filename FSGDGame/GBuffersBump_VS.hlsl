#ifndef GBUFFERS_BUMP_VS_HLSL
#define GBUFFERS_BUMP_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

GBufferBumpVertexOut main( VERTEX_POSNORMTANTEX input )
{
	GBufferBumpVertexOut output = (GBufferBumpVertexOut)0;
    // SOLUTION_BEGIN

	output.position = mul(float4(input.position, 1.0), gMVP);

	float4 normal4 = mul(float4(input.normal, 0.0), gWorld);
	float4 tangent4 = mul(float4(input.tangent.xyz, 0.0), gWorld);

	output.tbn[0] = tangent4.xyz;
	output.tbn[1] = cross(normal4.xyz, tangent4.xyz) * input.determinant;
	output.tbn[2] = normal4.xyz;

	output.texCoord = input.texCoord;

	output.depthDiv.xy = output.position.zw;

    // SOLUTION_END
	return output;
}

#endif //GBUFFERS_BUMP_VS_HLSL