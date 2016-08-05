#ifndef BRIGHT_PASS_PS_HLSL
#define BRIGHT_PASS_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

static const float gLuminance = 0.08f;
static const float sMiddleGray = 0.18f;
static const float sWhiteCutoff = 0.8f;

float4 main(TwoDVertexOut input) : SV_TARGET
{
	float2 texCoord = input.texCoord;
	
	float4 color = postSourceMap.Sample(linearClampSampler, texCoord);

    color *= sMiddleGray / ( gLuminance + 0.001f );
    color *= ( 1.0f + ( color / ( sWhiteCutoff * sWhiteCutoff ) ) );
    color -= 5.0f;

    color = max( color, 0.0f );

    color /= ( 10.0f + color );

	return color;
}

#endif //BRIGHT_PASS_PS_HLSL
