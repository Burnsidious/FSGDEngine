#ifndef GAUSSIAN_BLUR_PS_HLSL
#define GAUSSIAN_BLUR_PS_HLSL

#include "GaussianBlur.hlsli"

float4 main(BlurVertexOut inVert) : SV_TARGET
{
	return BlurPS(inVert, 1.0f);
}

#endif //GAUSSIAN_BLUR_PS_HLSL
