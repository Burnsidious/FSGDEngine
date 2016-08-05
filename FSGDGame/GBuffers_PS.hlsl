#ifndef GBUFFERS_PS_HLSL
#define GBUFFERS_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "GBuffersShared.hlsli"

GBufferFragOut main(GBufferVertexOut input)
{
	GBufferFragOut output = (GBufferFragOut)0;
    // SOLUTION_BEGIN
	output = CreateGBufferOutput(input.normalW, input.texCoord, input.depthDiv);

    // SOLUTION_END
	return output;
}

#endif //GBUFFERS_PS_HLSL