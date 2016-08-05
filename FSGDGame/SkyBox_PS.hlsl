#ifndef SKY_BOX_PS_HLSL
#define SKY_BOX_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"

float4 main(CubeVertOut inVert) : SV_TARGET
{
	return reflectedMap.Sample(linearClampSampler, inVert.texCoord);
}

#endif //SKY_BOX_PS_HLSL