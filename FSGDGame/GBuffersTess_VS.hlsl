#ifndef GBUFFERS_TESS_VS_HLSL
#define GBUFFERS_TESS_VS_HLSL

#include "VertexLayouts.hlsli"
#include "Tesselation.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

GBufferTessVertexOut main( VERTEX_POSNORMTEX input )
{
	GBufferTessVertexOut output = (GBufferTessVertexOut)0;
    // SOLUTION_BEGIN

	output.positionW = mul(float4(input.position, 1.0), gWorld).xyz;

	output.normalW = mul(float4(input.normal, 0.0), gWorld).xyz;

	output.texCoord = float2(input.texCoord.x, input.texCoord.y);

	output.tessFactor = DistanceBasedTesselation(output.positionW);
	
    // SOLUTION_END

	return output;
}

#endif //GBUFFERS_TESS_VS_HLSL