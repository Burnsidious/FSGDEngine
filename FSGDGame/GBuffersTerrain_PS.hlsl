#ifndef GBUFFERS_TERRAIN_PS_HLSL
#define GBUFFERS_TERRAIN_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"

//float4 main() : SV_TARGET
//{
//	return float4(1, 0, 0, 1);
//}

GBufferFragOut main(GBufferVertexOut input)
{
	GBufferFragOut fragOut;

	// Sample the three textures
	float4 foliageColor = diffuseMap.Sample(linearWrapSampler, input.texCoord);
	float4 earthColor = specularMap.Sample(linearWrapSampler, input.texCoord);
	float noise = normalMap.Sample(pointWrapSampler, input.texCoord).r - 0.5;

	fragOut.normal = float4(normalize(input.normalW) * .5f + .5f, 0);

	float lerpBias = pow(fragOut.normal.y, 8) - noise * .5;

	fragOut.diffuse = lerp(earthColor, foliageColor, lerpBias);

	fragOut.specular = float4(0, 0, 0, 0);

	fragOut.depth = input.depthDiv.x / input.depthDiv.y;

	return fragOut;
}

#endif //GBUFFERS_TERRAIN_PS_HLSL