#ifndef GBUFFERS_BUMP_TESS_VS_HLSL
#define GBUFFERS_BUMP_TESS_VS_HLSL

#include "VertexLayouts.hlsli"
#include "Tesselation.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"


GBufferBumpTessVertexOut main( VERTEX_POSNORMTANTEX input )
{
	GBufferBumpTessVertexOut output = (GBufferBumpTessVertexOut)0;
    // SOLUTION_BEGIN

	output.positionW = mul(float4(input.position, 1.0), gWorld).xyz;

	output.normalW = mul(float4(input.normal, 0.0), gWorld).xyz;
	output.tangentW = mul(float4(input.tangent.xyz, 0.0), gWorld).xyz;
	output.determinant = input.determinant;

	output.texCoord = float2(input.texCoord.x, input.texCoord.y);

	output.tessFactor = DistanceBasedTesselation(output.positionW);

    // SOLUTION_END
	return output;
}

#endif //GBUFFERS_BUMP_TESS_VS_HLSL