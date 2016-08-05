#ifndef GAUSSIAN_BLOOM_PS_HLSL
#define GAUSSIAN_BLOOM_PS_HLSL

#include "GaussianBlur.hlsli"

float4 main(BlurVertexOut inVert) : SV_TARGET
{
	return BlurPS(inVert, 1.5f);
}

#endif //GAUSSIAN_BLOOM_PS_HLSL
