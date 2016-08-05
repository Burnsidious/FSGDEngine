#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"


float4 main(TwoDVertexOut input) : SV_TARGET
{
	input.texCoord.y += sin(
		gTheta + input.texCoord.x * gScale);

	return postSourceMap.Sample(linearWrapSampler, 
		input.texCoord) * gFade;
}