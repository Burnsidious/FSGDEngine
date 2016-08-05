#ifndef DRAW_EDGE_NORM_SOBEL_PS_HLSL
#define DRAW_EDGE_NORM_SOBEL_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

static float theNormalThreshold = 0.2;

float4 main(TwoDVertexOut input) : SV_TARGET0
{
	float4 sceneColor = postSourceMap.SampleLevel(linearClampSampler, input.texCoord, 0);

	float normTL =	dot(.3333, normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2(-1, -1)).rgb);
	float normT =	dot(.3333, normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 0, -1)).rgb);
	float normTR =	dot(.3333, normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 1, -1)).rgb);
	float normL =	dot(.3333, normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2(-1,  0)).rgb);
	float normR =	dot(.3333, normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 1,  0)).rgb);
	float normBL =	dot(.3333, normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2(-1,  1)).rgb);
	float normB =	dot(.3333, normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 0,  1)).rgb);
	float normBR =	dot(.3333, normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 1,  1)).rgb);

	float horizontalChange = normTL + 2 * normL + normBL - normTR - 2 * normR - normBR;
	float verticleChange = normTL + 2 * normT + normTR - normBL - 2 * normB - normBR;

	float sqChange = horizontalChange * horizontalChange + verticleChange * verticleChange;
	float edgeAmount = sqChange > theNormalThreshold * theNormalThreshold; 

	// Darken color based on edgy-ness
    sceneColor *= (1 - edgeAmount);

    return float4(sceneColor.rgb, 1);
}

#endif //DRAW_EDGE_NORM_SOBEL_PS_HLSL