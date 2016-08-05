#ifndef GBUFFERS_VS_HLSL
#define GBUFFERS_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

GBufferVertexOut main( VERTEX_POSNORMTEX input )
{
	GBufferVertexOut output = (GBufferVertexOut)0;
    // SOLUTION_BEGIN

	output.position = mul(float4(input.position, 1.0), gMVP);

	output.normalW = mul(float4(input.normal, 0.0), gWorld).xyz;

	output.texCoord = input.texCoord;

	output.depthDiv.xy = output.position.zw;
    // SOLUTION_END

	return output;
}

#endif //GBUFFERS_VS_HLSL