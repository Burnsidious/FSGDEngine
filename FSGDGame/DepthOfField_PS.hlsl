#ifndef DEPTH_OF_FIELD_PS_HLSL
#define DEPTH_OF_FIELD_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

//const static float gFocusDistance = 15.0f;
//const static float gFocusRange = 50.0f;

float4 main(TwoDVertexOut input) : SV_TARGET
{
	float4 sharpColor, blurColor, finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float depth, camDepth, blur;
    // SOLUTION_BEGIN
    float2 texCoord = input.texCoord;

	sharpColor = prePostMap.Sample(linearClampSampler, texCoord);
	blurColor = postSourceMap.Sample(linearClampSampler, texCoord);
	depth = depthGBuffer.Sample(pointClampSampler, texCoord).r;

	camDepth = gPlaneY / (gPlaneX + depth);

	blur = saturate( abs( camDepth - gFocusDistance ) / gFocusRange );

	finalColor.rgb = lerp(sharpColor.rgb, blurColor.rgb, blur);

	finalColor.a = 1.0;
    // SOLUTION_END
	return finalColor;
}

#endif //DEPTH_OF_FIELD_PS_HLSL
