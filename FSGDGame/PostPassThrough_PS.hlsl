#ifndef POST_PASS_THROUGH_PS_HLSL
#define POST_PASS_THROUGH_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

float4 main(TwoDVertexOut input) : SV_TARGET
{
	float2 texCoord = input.texCoord;
	return postSourceMap.Sample(linearClampSampler, texCoord);
	//return float4( 1, postSourceMap.Sample(linearClampSampler, texCoord).gba);
}

#endif //POST_PASS_THROUGH_PS_HLSL
