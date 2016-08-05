#ifndef GBUFFERS_DS_HLSL
#define GBUFFERS_DS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"
#include "TexturesAndSamplers.hlsli"

#define NUM_CONTROL_POINTS 3

[domain("tri")]
GBufferVertexOut main(
	HullConstTriOut input,
	float3 barycentric : SV_DomainLocation,
	const OutputPatch<GBufferHullOut, NUM_CONTROL_POINTS> patch)
{
	GBufferVertexOut output;

	// Interpolate position
	output.position.xyz = 
		patch[0].positionW * barycentric.x +
		patch[1].positionW * barycentric.y +
		patch[2].positionW * barycentric.z;
	output.position.w = 1;

	// Interpolate and normalize normal
	output.normalW = normalize(
		patch[0].normalW * barycentric.x + 
		patch[1].normalW * barycentric.y +
		patch[2].normalW * barycentric.z);

	// Interpolate texture coordinate
	output.texCoord.xy = 
		patch[0].texCoord * barycentric.x +
		patch[1].texCoord * barycentric.y +
		patch[2].texCoord * barycentric.z;

	output.depthDiv.xy = output.position.zw;

	// Displacement mapping goes here
	float height =/* 1.0 - */diffuseMap.SampleLevel(anisoWrapSampler, output.texCoord, 0).r;
	height -= 0.5;
	
	output.position.xyz = output.position.xyz + output.normalW * height * gDisplacementScale;

	output.position = mul(output.position, gViewProj);

	return output;
}

#endif //GBUFFERS_DS_HLSL
