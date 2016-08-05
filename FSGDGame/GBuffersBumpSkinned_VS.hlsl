#ifndef GBUFFERS_BUMP_SKINNED_VS_HLSL
#define GBUFFERS_BUMP_SKINNED_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

GBufferBumpVertexOut main( VERTEX_POSBONEWEIGHTNORMTANTEX input )
{
	GBufferBumpVertexOut output = (GBufferBumpVertexOut)0;
    // SOLUTION_BEGIN

	// Animation yo! Just dooo it
	float4x4 skinMat = gSkinnedMatrices[input.bone.x] * input.weights.x;
	skinMat += gSkinnedMatrices[input.bone.y] * input.weights.y;
	skinMat += gSkinnedMatrices[input.bone.z] * input.weights.z;
	skinMat += gSkinnedMatrices[input.bone.w] * input.weights.w;

	output.position = mul(float4(input.position, 1.0), skinMat);

	output.position = mul(output.position, gSkinnedViewProj);

	float4 normal4 = mul(float4(input.normal, 0.0), skinMat);
	float4 tangent4 = mul(float4(input.tangent.xyz, 0.0), skinMat);

	output.tbn[0] = tangent4.xyz;
	output.tbn[1] = cross(normal4.xyz, tangent4.xyz) * input.determinant;
	output.tbn[2] = normal4.xyz;

	output.texCoord = input.texCoord;

	output.depthDiv.xy = output.position.zw;
    // SOLUTION_END

	return output;
}

#endif //GBUFFERS_BUMP_SKINNED_VS_HLSL