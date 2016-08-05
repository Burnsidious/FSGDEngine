#ifndef DRAW_EDGE_PS_HLSL
#define DRAW_EDGE_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

static float theNormalThreshold = 0.25;
static float theDepthThreshold = 0.005;

static float theNormalSensitivity = 1;
static float theDepthSensitivity = 10;

float4 main(TwoDVertexOut input) : SV_TARGET0
{
	float4 sceneColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 normalTL, normalTR, normalBR, normalBL, diagonalDeltaNorm;
	float depthTL, depthTR, depthBR, depthBL, normalDelta, depthDelta, edgeAmount;

	// SOLUTION_BEGIN
	sceneColor = postSourceMap.SampleLevel(linearClampSampler, input.texCoord, 0);

	normalTL = normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2(-1, -1));
	normalTR = normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 1, -1));
	normalBR = normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 1,  1));
	normalBL = normalGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2(-1,  1));

	depthTL = depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2(-1, -1)).r;
	depthTR = depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 1, -1)).r;
	depthBR = depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2( 1,  1)).r;
	depthBL = depthGBuffer.SampleLevel(pointClampSampler, input.texCoord, 0, float2(-1,  1)).r;

	// Calculate change amounts
    diagonalDeltaNorm = abs(normalTL - normalBR) + abs(normalTR - normalBL);
    normalDelta = dot(diagonalDeltaNorm.xyz, 1);
	depthDelta = abs(depthTL - depthBR) + abs(depthTR - depthBL);
    
    // Filter out changes below thresholds
    normalDelta = saturate((normalDelta - theNormalThreshold) * (theNormalSensitivity * gEdgeWidth));
	depthDelta = saturate((depthDelta - theDepthThreshold) * (theDepthSensitivity)* gEdgeWidth);

    // Calculate how "edgy" the given texel is
    edgeAmount = ((normalDelta + depthDelta) * gEdgeIntensity) * 0.5f;

	// Darken color based on edgy-ness
    sceneColor *= (1 - edgeAmount);
    // SOLUTION_END

    return float4(sceneColor.rgb, 1);
}
//87
#endif //DRAW_EDGE_PS_HLSL