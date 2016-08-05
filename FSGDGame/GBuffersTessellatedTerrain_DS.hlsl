#ifndef GBUFFERS_TESSELLATED_TERRAIN_DS_HLSL
#define GBUFFERS_TESSELLATED_TERRAIN_DS_HLSL

#include "VertexLayouts.hlsli"
#include "TerrainSupport.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

#define NUM_CONTROL_POINTS 4

// Method for bilinear interpolation
float3 Bilerp(float3 v0, float3 v1, float3 v2, float3 v3, float2 i)
{
	// SOLUTION_BEGIN

    float3 bottom = lerp(v0, v3, i.x);
    float3 top    = lerp(v1, v2, i.x);
    return lerp(bottom, top, i.y);
	// SOLUTION_END

	return float3(0, 0, 0);
}

float3 TessellatedWorldPos(
	float2 UV : SV_DomainLocation,
	const OutputPatch<TESS_TERRAIN_HS_OUT, 4> terrainQuad)
{
	float3 worldPos;
	// SOLUTION_BEGIN

	// bilerp the position
	worldPos = Bilerp(terrainQuad[0].position, terrainQuad[1].position,
		terrainQuad[2].position, terrainQuad[3].position, UV);

	worldPos.y = CalculateTerrainHeight(worldPos.xz);

	// SOLUTION_END

	return worldPos;
}

// From nVidia
// The domain shader is run once per vertex and calculates the final vertex's position
// and attributes.  It receives the UVW from the fixed function tessellator and the
// control point outputs from the hull shader.  Since we are using the DirectX 11
// Tessellation pipeline, it is the domain shader's responsibility to calculate the
// final SV_POSITION for each vertex.

// The input SV_DomainLocation to the domain shader comes from fixed function
// tessellator.  And the OutputPatch comes from the hull shader.  From these, you
// must calculate the final vertex position, color, texcoords, and other attributes.

// The output from the domain shader will be a vertex that will go to the video card's
// rasterization pipeline and get drawn to the screen.
[domain("quad")]
TESS_TERRAIN_DS_OUT main(
	TESS_TERRAIN_HS_DATA_OUT input,
	float2 UV : SV_DomainLocation,
	const OutputPatch<TESS_TERRAIN_HS_OUT, NUM_CONTROL_POINTS> terrainQuad)
{
	TESS_TERRAIN_DS_OUT Output = (TESS_TERRAIN_DS_OUT)0;
	// SOLUTION_BEGIN

	// Perform bi-linear interpolation for output position, and sample height from map
	const float3 worldPos = TessellatedWorldPos(UV, terrainQuad);

	// Apply view projection matrix
    Output.position = mul(float4(worldPos.xyz,1), gTileMVP);

	// X,Z positional data is used by pixel shader to determine texture coordinates
	Output.worldXZ = worldPos.xz;

	// Calculate depth for writing to depth GBuffer
	Output.depthDiv = Output.position.zw;

	// SOLUTION_END

	// Interpolate nearby debug transition colors
	Output.debugColor = lerpDebugColors(input.debugColor, UV);

	return Output;    
}

#endif //GBUFFERS_TESSELLATED_TERRAIN_DS_HLSL