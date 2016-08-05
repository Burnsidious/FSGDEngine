#ifndef FAR_CLIP_POSITION_VS_HLSL
#define FAR_CLIP_POSITION_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

VertexWithPositionOut main(float3 position : POSITION0)
{
	VertexWithPositionOut output;
	
	output.position = mul(float4(position, 1.0), gMVP);
	output.position.z = min(output.position.w, output.position.z);

	output.pixelPosition = output.position;

	return output;
}

#endif //FAR_CLIP_POSITION_VS_HLSL