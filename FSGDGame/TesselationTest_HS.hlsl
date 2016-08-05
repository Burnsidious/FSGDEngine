#ifndef TESSELATION_TEST_HS_HLSL
#define TESSELATION_TEST_HS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<TESSELATION_TEST_VS_OUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	float3 centroidPos = 0.33f * (ip[0].position + ip[1].position + ip[2].position);
		float dist = distance(centroidPos, gCameraPos);

	const float distMin = 1.0f;
	const float distMax = 25.0f;
	float tess = max(1, 12.0f * saturate((distMax-dist) / (distMax - distMin)));

	// Insert code to compute Output here
	Output.EdgeTessFactor[0] = tess;
	Output.EdgeTessFactor[1] = tess;
	Output.EdgeTessFactor[2] = tess;
	Output.InsideTessFactor = tess;//15; // e.g. could calculate dynamic tessellation factors instead

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
TESSELATION_TEST_VS_OUT main( 
	InputPatch<TESSELATION_TEST_VS_OUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	TESSELATION_TEST_VS_OUT output;

	// Insert code to compute Output here
	output.position = ip[i].position;
	output.normal = ip[i].normal;
	output.texCoord = ip[i].texCoord;

	return output;
}

#endif //TESSELATION_TEST_HS_HLSL
