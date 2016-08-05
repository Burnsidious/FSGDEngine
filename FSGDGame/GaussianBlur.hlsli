#ifndef GAUSSIAN_BLUR_HLSLI
#define GAUSSIAN_BLUR_HLSLI

#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

const static float3 gBlurWeights = float3(0.24197073, 0.053990975, 0.0044318493);
const static float gCenterBlurWeights = 0.39894229f;

struct BlurVertexOut
{
	float4 position : SV_POSITION0;
	float2 texCoord : TEXCOORD0;
	float2 positiveOffsets[3] : TEXCOORD1;
    float2 negativeOffsets[3] : TEXCOORD4;
};

BlurVertexOut BlurVS(float3 position : POSITION0, float2 texCoord : TEXCOORD0,
	uniform float2 direction)
{
	BlurVertexOut outVert = (BlurVertexOut)0;
    // SOLUTION_BEGIN
	float2 pixelSize = gHalfPixel * 2;

	outVert.position = float4(position, 1);
	outVert.texCoord = texCoord * gTexCoordScale;

	outVert.positiveOffsets[0] = outVert.texCoord + direction * pixelSize;
	outVert.positiveOffsets[1] = outVert.texCoord + 2 * direction * pixelSize;
	outVert.positiveOffsets[2] = outVert.texCoord + 3 * direction * pixelSize;

	outVert.negativeOffsets[0] = outVert.texCoord - direction * pixelSize;
	outVert.negativeOffsets[1] = outVert.texCoord - 2 * direction * pixelSize;
	outVert.negativeOffsets[2] = outVert.texCoord - 3 * direction * pixelSize;

    // SOLUTION_END
	return outVert;
}

float4 BlurPS(BlurVertexOut inVert, uniform float ScaleUp) : COLOR0
{
	float3x4 positiveSamples, negativeSamples;
	float4 color = float4(0, 0, 0, 0);
    // SOLUTION_BEGIN

	color = postSourceMap.Sample(linearClampSampler, inVert.texCoord) * gCenterBlurWeights;

	positiveSamples[0] = postSourceMap.Sample(linearClampSampler, inVert.positiveOffsets[0]);
	positiveSamples[1] = postSourceMap.Sample(linearClampSampler, inVert.positiveOffsets[1]);
	positiveSamples[2] = postSourceMap.Sample(linearClampSampler, inVert.positiveOffsets[2]);

	negativeSamples[0] = postSourceMap.Sample(linearClampSampler, inVert.negativeOffsets[0]);
	negativeSamples[1] = postSourceMap.Sample(linearClampSampler, inVert.negativeOffsets[1]);
	negativeSamples[2] = postSourceMap.Sample(linearClampSampler, inVert.negativeOffsets[2]);

	color += mul(gBlurWeights, positiveSamples) + mul(gBlurWeights, negativeSamples);	
    // SOLUTION_END

	return color * ScaleUp;
}


#endif //GAUSSIAN_BLUR_HLSLI