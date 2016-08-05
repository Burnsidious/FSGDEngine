#ifndef UP_SAMPLE_PS_HLSL
#define UP_SAMPLE_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

float4 main(TwoDVertexOut input) : SV_TARGET
{
	float2 texCoord = input.texCoord;
	//float2 texCoord = input.texCoord /2;
	return postSourceMap.Sample(linearClampSampler, texCoord);
	//return float4(postSourceMap.Sample(linearClampSampler, texCoord).rg, 1, 1);
}

#endif //UP_SAMPLE_PS_HLSL
