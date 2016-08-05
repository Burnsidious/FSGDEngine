#ifndef GBUFFERS_TESSELLATED_TERRAIN_PS_HLSL
#define GBUFFERS_TESSELLATED_TERRAIN_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "TerrainSupport.hlsli"

GBufferFragOut main(TESS_TERRAIN_DS_OUT input)
{
	GBufferFragOut fragOut = (GBufferFragOut)0;

	float2 heightTexCoord = CalculateTerrainHeightTexCoord(input.worldXZ);
	float2 diffuseTexCoord = CalculateTerrainDiffuseTexCoord(input.worldXZ);

	fragOut.normal = float4(courseHeightMap.SampleLevel(linearWrapSampler, heightTexCoord, 0).rgb, 0);
	//fragOut.normal = float4(0, 1, 0, 0);

	// Sample the three textures
	float4 foliageColor = diffuseMap1.Sample(linearWrapSampler, diffuseTexCoord);
	float4 earthColor = diffuseMap2.Sample(linearWrapSampler, diffuseTexCoord);
	float noise = diffuseNoise.Sample(linearWrapSampler, diffuseTexCoord).r - 0.5;
	
	float lerpBias = pow(fragOut.normal.y, 8) - noise * .5;

	fragOut.diffuse = lerp(earthColor, foliageColor, lerpBias);
	//fragOut.diffuse = (courseHeightMap.SampleLevel(linearWrapSampler, heightTexCoord, 0).r);
	//fragOut.diffuse = float4(0, 0, 0, 1);
	if(gShowTerrainTransistions)
		fragOut.diffuse = float4(input.debugColor, 1);


	fragOut.specular = float4(0, 0, 0, 0);

	fragOut.depth = input.depthDiv.x / input.depthDiv.y;

	return fragOut;
}

#endif //GBUFFERS_TESSELLATED_TERRAIN_PS_HLSL