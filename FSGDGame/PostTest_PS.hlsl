#ifndef POST_TEST_PS_HLSL
#define POST_TEST_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

float4 main(TwoDVertexOut input) : SV_TARGET0
{
	float4 color;
    float2 texCoord = input.texCoord;

	color = postSourceMap.Sample(linearClampSampler, texCoord);

	return color.rrba;
}

#endif //POST_TEST_PS_HLSL
























	/*color = postSourceMap.Sample(linearClampSampler, texCoord);
	float3 retCol;
	retCol.r = dot(float3(.393, .769, .189), color.rgb);
	retCol.g = dot(float3(.349, .686, .168), color.rgb);
	retCol.b = dot(float3(.272, .534, .131), color.rgb);
	return float4(retCol, 1);*/