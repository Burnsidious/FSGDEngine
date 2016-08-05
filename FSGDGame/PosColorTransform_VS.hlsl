#ifndef POS_COLOR_TRANSFORM_VS_HLSL
#define POS_COLOR_TRANSFORM_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

PosColorOut main( VERTEX_POSCOLOR input)
{
	PosColorOut output;

	output.position = mul(float4(input.position.xyz, 1), gMVP);

	output.color = input.color;

	return output;
}

#endif //POS_COLOR_TRANSFORM_VS_HLSL