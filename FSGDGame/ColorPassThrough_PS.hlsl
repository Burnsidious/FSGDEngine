#ifndef COLOR_PASS_THROUGH_PS_HLSL
#define COLOR_PASS_THROUGH_PS_HLSL

#include "VertexLayouts.hlsli"

float4 main(PosColorOut input) : SV_TARGET0
{
	return input.color;
}

#endif //COLOR_PASS_THROUGH_PS_HLSL