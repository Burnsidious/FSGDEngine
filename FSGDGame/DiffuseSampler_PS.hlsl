#ifndef DIFFUSE_SAMPLER_PS_HLSL
#define DIFFUSE_SAMPLER_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"

float4 main(TwoDVertexOut input) : SV_TARGET
{
	return diffuseMap.Sample(pointWrapSampler, input.texCoord);
}

#endif //DIFFUSE_SAMPLER_PS_HLSL