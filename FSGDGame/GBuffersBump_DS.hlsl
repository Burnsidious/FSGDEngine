#ifndef GBUFFERS_BUMP_DS_HLSL
#define GBUFFERS_BUMP_DS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"
#include "TexturesAndSamplers.hlsli"

#define NUM_CONTROL_POINTS 3

[domain("tri")]
GBufferBumpVertexOut main(
	HullConstTriOut input,
	float3 barycentric : SV_DomainLocation,
	const OutputPatch<GBufferBumpTessHullOut, NUM_CONTROL_POINTS> patch)
{
	GBufferBumpVertexOut output;

	// Interpolate position
	output.position.xyz = 
		patch[0].positionW * barycentric.x +
		patch[1].positionW * barycentric.y +
		patch[2].positionW * barycentric.z;
	output.position.w = 1;

	// Interpolate and normalize normal
	output.tbn[2] = normalize(
		patch[0].normalW * barycentric.x +
		patch[1].normalW * barycentric.y +
		patch[2].normalW * barycentric.z);

	output.tbn[0] = normalize(
		patch[0].tangentW.xyz * barycentric.x +
		patch[1].tangentW.xyz * barycentric.y +
		patch[2].tangentW.xyz * barycentric.z);

	output.tbn[1] = cross(output.tbn[2], output.tbn[0]) * patch[0].determinant;

	// Interpolate texture coordinate
	output.texCoord.xy = 
		patch[0].texCoord * barycentric.x +
		patch[1].texCoord * barycentric.y +
		patch[2].texCoord * barycentric.z;

	output.depthDiv.xy = output.position.zw;

	// Displacement mapping goes here
	float height = /*1.0 - */diffuseMap.SampleLevel(anisoWrapSampler, output.texCoord, 0).r;
	height -= 0.5;

	//float3 normal = normalMap.SampleLevel(anisoWrapSampler, output.texCoord, 0).xyz;
	//normal = (normal * 2)-1;
	//normal = normalize(mul(normal, output.tbn));

	float3 normal = (normalMap.SampleLevel(linearClampSampler, output.texCoord, 0).rgb * 2 - 1.0f);

	normal = normalize(mul(normal, output.tbn));

	output.position.xyz = output.position.xyz + normal.xyz * height * gDisplacementScale;


	output.position = mul(output.position, gViewProj);

	//output.normal = mul(float4(output.normal.xyz, 0), gWorld);


	return output;
}

#endif //GBUFFERS_BUMP_DS_HLSL
