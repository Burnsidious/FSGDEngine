#ifndef SKETCH_PS_HLSL
#define SKETCH_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

float SketchThreshold = 0.1;
float SketchBrightness = 0.333;

float4 main(TwoDVertexOut input) : SV_TARGET
{
	float4 sceneColor = postSourceMap.Sample(linearClampSampler, input.texCoord);

	float3 noiseColor = postTextureMap.Sample(linearWrapSampler, 
		input.texCoord * .5 + gJitter).rgb;

	float3 saturatedScene = saturate((sceneColor.rgb - 0.0) * 2);

	float3 negativeSketch = (1 - saturatedScene) * (1 - noiseColor);

	float sketchResult = dot(1 - negativeSketch, 0.5);

	//return dot(sceneColor, float3(0.2, 0.5, 0.3)) * sketchResult;
	return sceneColor * sketchResult;
}

#endif //SKETCH_PS_HLSL