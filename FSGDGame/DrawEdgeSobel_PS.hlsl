#ifndef DRAW_EDGE_SOBEL_PS_HLSL
#define DRAW_EDGE_SOBEL_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

static float theDepthThreshold = 0.003;

float4 main(TwoDVertexOut input) : SV_TARGET0
{
	float4 sceneColor = postSourceMap.SampleLevel(linearClampSampler, input.texCoord, 0);

	float depthTL = depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2(-1, -1)).r;
	float depthT =	depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 0, -1)).r;
	float depthTR = depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 1, -1)).r;
	float depthL =	depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2(-1,  0)).r;
	float depthR =	depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 1,  0)).r;
	float depthBL = depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2(-1,  1)).r;
	float depthB =	depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 0,  1)).r;
	float depthBR = depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 1,  1)).r;

	float horizontalChange = depthTL + 2 * depthL + depthBL - depthTR - 2 * depthR - depthBR;
	float verticleChange = depthTL + 2 * depthT + depthTR - depthBL - 2 * depthB - depthBR;

	float sqChange = horizontalChange * horizontalChange + verticleChange * verticleChange;
	float edgeAmount = sqChange > theDepthThreshold * theDepthThreshold; 

	// Darken color based on edgy-ness
    sceneColor *= (1 - edgeAmount);

    return float4(sceneColor.rgb, 1);
}

#endif //DRAW_EDGE_SOBEL_PS_HLSL