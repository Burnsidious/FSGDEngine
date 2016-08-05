#ifndef TESSELATION_TEST_DS_HLSL
#define TESSELATION_TEST_DS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"
#include "TexturesAndSamplers.hlsli"

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
TESSELATION_TEST_HS_OUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<TESSELATION_TEST_VS_OUT, NUM_CONTROL_POINTS> patch)
{
	TESSELATION_TEST_HS_OUT output;

	// Interpolate position
	output.position = float4(
		patch[0].position*domain.x+patch[1].position*domain.y+patch[2].position*domain.z,1);

	// Interpolate and normalize normal
	output.normal = normalize(
		patch[0].normal*domain.x+patch[1].normal*domain.y+patch[2].normal*domain.z);

	// Interpolate texture coordinate
	output.texCoord = patch[0].texCoord*domain.x+patch[1].texCoord*domain.y+patch[2].texCoord*domain.z;

	float height = diffuseMap.SampleLevel(anisoWrapSampler, output.texCoord, 0).r;
	output.position += (.5 * (height - 1.0f)) * output.normal;

	output.position = mul(output.position, gMVP);

	output.normal = mul(float4(output.normal.xyz, 0), gWorld);


	return output;
}

#endif //TESSELATION_TEST_DS_HLSL
