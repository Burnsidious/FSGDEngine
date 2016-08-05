#ifndef GBUFFERS_SKINNED_VS_HLSL
#define GBUFFERS_SKINNED_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

GBufferVertexOut main( VERTEX_POSBONEWEIGHTNORMTEX input )
{
	GBufferVertexOut output = (GBufferVertexOut)0;
    // SOLUTION_BEGIN

	// Animation yo! Just dooo it
	float4x4 skinMat = gSkinnedMatrices[input.bone.x] * input.weights.x;
	skinMat += gSkinnedMatrices[input.bone.y] * input.weights.y;
	skinMat += gSkinnedMatrices[input.bone.z] * input.weights.z;
	skinMat += gSkinnedMatrices[input.bone.w] * input.weights.w;

	output.position = mul(float4(input.position, 1.0), skinMat);

	output.position = mul(output.position, gSkinnedViewProj);

	output.normalW = mul(float4(input.normal, 0.0), skinMat).xyz;

	output.texCoord = input.texCoord;

	output.depthDiv.xy = output.position.zw;
    // SOLUTION_END

	return output;
}

#endif //GBUFFERS_SKINNED_VS_HLSL