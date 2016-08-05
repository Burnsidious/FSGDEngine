#ifndef GBUFFERS_TESS_HS_HLSL
#define GBUFFERS_TESS_HS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

#define NUM_CONTROL_POINTS 3

// Patch Constant Function
HullConstTriOut CalcHSPatchConstants(
	InputPatch<GBufferTessVertexOut, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HullConstTriOut Output;

	// Insert code to compute Output here
	Output.EdgeTessFactor[0] = 0.5f * (ip[1].tessFactor + ip[2].tessFactor);
	Output.EdgeTessFactor[1] = 0.5f * (ip[0].tessFactor + ip[2].tessFactor);
	Output.EdgeTessFactor[2] = 0.5f * (ip[0].tessFactor + ip[1].tessFactor);
	Output.InsideTessFactor = ip[0].tessFactor; 

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
GBufferHullOut main( 
	InputPatch<GBufferTessVertexOut, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	GBufferHullOut Output;

	// Insert code to compute Output here
	Output.positionW = ip[i].positionW;
	Output.normalW = ip[i].normalW;
	Output.texCoord = ip[i].texCoord;

	return Output;
}

#endif //GBUFFERS_TESS_HS_HLSL
