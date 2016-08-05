#ifndef POST_COMBINE_ADD_PS_HLSL
#define POST_COMBINE_ADD_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

float4 main(TwoDVertexOut input) : SV_TARGET
{
	float2 texCoord = input.texCoord;
	
	return postSourceMap.Sample(linearClampSampler, texCoord) + 
		prePostMap.Sample(linearClampSampler, texCoord);
}

#endif //POST_COMBINE_ADD_PS_HLSL
