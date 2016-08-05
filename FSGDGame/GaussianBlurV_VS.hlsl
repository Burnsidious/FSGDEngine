#ifndef GAUSSIAN_BLUR_V_VS_HLSL
#define GAUSSIAN_BLUR_V_VS_HLSL

#include "GaussianBlur.hlsli"

BlurVertexOut main( float3 position : POSITION0, float2 texCoord : TEXCOORD0 )
{
    // SOLUTION_BEGIN
    // SOLUTION_END

	return BlurVS(position, texCoord, float2(0, 1));
}

#endif //GAUSSIAN_BLUR_V_VS_HLSL
