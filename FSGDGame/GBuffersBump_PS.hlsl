#ifndef GBUFFERS_BUMP_PS_HLSL
#define GBUFFERS_BUMP_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"
#include "GBuffersShared.hlsli"

GBufferFragOut main(GBufferBumpVertexOut input)
{
	GBufferFragOut output = (GBufferFragOut)0;
    // SOLUTION_BEGIN
	float4 norm = normalMap.Sample(anisoWrapSampler, input.texCoord) * 2 - 1.0f;

	norm = (float4(mul(norm.xyz, input.tbn), 0));

	output = CreateGBufferOutput(norm.xyz, input.texCoord, input.depthDiv);

	// SOLUTION_END
	return output;
}

#endif //GBUFFERS_BUMP_PS_HLSL