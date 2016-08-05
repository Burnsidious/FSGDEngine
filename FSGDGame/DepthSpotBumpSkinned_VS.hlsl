#ifndef DEPTH_SPOT_BUMP_SKINNED_VS_HLSL
#define DEPTH_SPOT_BUMP_SKINNED_VS_HLSL

#include "../EDRendererD3D/ConstantBuffers.h"
#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/LightBuffers.h"

DepthVertOut main( VERTEX_POSBONEWEIGHTNORMTANTEX input )
{
	DepthVertOut output;
	
	float4x4 skinMat = gSkinnedMatrices[input.bone.x] * input.weights.x;
	skinMat += gSkinnedMatrices[input.bone.y] * input.weights.y;
	skinMat += gSkinnedMatrices[input.bone.z] * input.weights.z;
	skinMat += gSkinnedMatrices[input.bone.w] * input.weights.w;

	output.position = mul(float4(input.position, 1.0), skinMat);

	output.position = mul(output.position, spotLightShadowData.viewProjection);

    output.depth.xy = output.position.zw;
	output.texCoord = input.texCoord;

	return output;
}

#endif //DEPTH_SPOT_BUMP_SKINNED_VS_HLSL