#ifndef GBUFFERS_BUMP_HS_HLSL
#define GBUFFERS_BUMP_HS_HLSL

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
	InputPatch<GBufferBumpTessVertexOut, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT output;

	// Insert code to compute Output here
	output.EdgeTessFactor[0] = 0.5f * (ip[1].tessFactor + ip[2].tessFactor);
	output.EdgeTessFactor[1] = 0.5f * (ip[0].tessFactor + ip[2].tessFactor);
	output.EdgeTessFactor[2] = 0.5f * (ip[0].tessFactor + ip[1].tessFactor);
	output.InsideTessFactor = ip[0].tessFactor; 

	return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
GBufferBumpTessHullOut main( 
	InputPatch<GBufferBumpTessVertexOut, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	GBufferBumpTessHullOut output;

	// Insert code to compute Output here
	output.positionW = ip[i].positionW;
	output.normalW = ip[i].normalW;
	output.tangentW = ip[i].tangentW;
	output.determinant = ip[i].determinant;
	output.texCoord = ip[i].texCoord;

	return output;
}

#endif //GBUFFERS_BUMP_HS_HLSL
