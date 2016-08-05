#ifndef DEPTH_POINT_SKINNED_VS_HLSL
#define DEPTH_POINT_SKINNED_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

PointDepthVertOut main(VERTEX_POSBONEWEIGHTNORMTEX input)
{
	PointDepthVertOut output;

	float4x4 skinMat = gSkinnedMatrices[input.bone.x] * input.weights.x;
	skinMat += gSkinnedMatrices[input.bone.y] * input.weights.y;
	skinMat += gSkinnedMatrices[input.bone.z] * input.weights.z;
	skinMat += gSkinnedMatrices[input.bone.w] * input.weights.w;

	output.position = mul(float4(input.position, 1.0), skinMat);
	output.normalW = mul(float4(input.normal, 0.0), skinMat).xyz;
	
	output.texCoord = input.texCoord;

	return output;
}

#endif //DEPTH_POINT_SKINNED_VS_HLSL