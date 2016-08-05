#ifndef DEPTH_PS_HLSL
#define DEPTH_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"

float4 main(DepthVertOut input) : SV_TARGET0
{
    // SOLUTION_BEGIN
	if (diffuseMap.SampleLevel(pointWrapSampler, input.texCoord, 0).a < MIN_ALPHA_KEEP)
		discard;

	return input.depth.x / input.depth.y;
    // SOLUTION_END
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

#endif //DEPTH_PS_HLSL