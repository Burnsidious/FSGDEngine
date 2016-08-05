#ifndef GBUFFERS_TESSELLATED_TERRAIN_HS
#define GBUFFERS_TESSELLATED_TERRAIN_HS

#include "VertexLayouts.hlsli"
#include "TerrainSupport.hlsli"

#include "../EDRendererD3D/ConstantBuffers.h"

// Work assumes a "quad" like patch
#define NUM_CONTROL_POINTS 4

// Projects a sphere diameter large in screen space to calculate desired tesselation factor
float SphereToScreenSpaceTessellation(float3 p0, float3 p1, float diameter)
{
	int ret = 1;

	// SOLUTION_BEGIN
	// Calculate center point
	float3 center = 0.5 * (p0+p1);
	// Put center point in view space
	float4 view0 = mul(float4(center,1), gTileWorldView);

	// Find a view space point diameter distance away
	float4 view1 = view0;
	view1.x += diameter;

	// Apply projection to both points
	float4 clip0 = mul(view0, gTileProj);
	float4 clip1 = mul(view1, gTileProj);

	// Put into clip space
	clip0 /= clip0.w;
	clip1 /= clip1.w;

	// Put into screen  space coordinates
	clip0.xy *= gScreenSize;
	clip1.xy *= gScreenSize;

	// Find screen space distance
	const float d = distance(clip0, clip1);

	// base tesselation on ratio of current screen space size over desired size, clamped [0, 64]
	ret = clamp(d / gTessellatedTriWidth, 1, 64);
	// SOLUTION_END
	return ret;
}

// A conservative test if a patch is in the frustum viewing area, false positives are possible
int inFrustum(const float3 center, float diameter)
{
	int ret = 0;

	// SOLUTION_BEGIN
	// conservative frustum culling
	float3 eyeToPt = center - gCameraPos;

	// Find the vector from the center of the patch, to the nearest point on the view direction
	float3 centerToNearestPointOnView = gCameraDir * dot(eyeToPt, gCameraDir) - eyeToPt;

	// Find the closest point on sphere to view direction, could be inside the sphere
	float3 closestPointOnSphere = center + normalize(centerToNearestPointOnView) 
		* min(diameter, length(centerToNearestPointOnView));


	// Project this point to test if it is on screen
	float4 projectedClosestPointOnSphere = mul(float4(closestPointOnSphere, 1.0), gTileMVP);

	// Test if the projected point is in view. The point will be in view if it's clip space x and y are between -1 and +1, while the z is between 0 and +1 or
	// if the distance to the center is less than the radius of the sphere. 
	if(((projectedClosestPointOnSphere.x/projectedClosestPointOnSphere.w > -1.0) && (projectedClosestPointOnSphere.x/projectedClosestPointOnSphere.w < 1.0) &&
		(projectedClosestPointOnSphere.y/projectedClosestPointOnSphere.w > -1.0) && (projectedClosestPointOnSphere.y/projectedClosestPointOnSphere.w < 1.0) &&
		(projectedClosestPointOnSphere.w>0)) || (length(center-gCameraPos) < diameter))
	{
		ret = 1;
	}
	// SOLUTION_END

	return ret;
}

// Patch Constant Function
// This method will cull patches outside of view, and set tesselation factotrs for in view patches
TESS_TERRAIN_HS_DATA_OUT CalcHSPatchConstants(
	InputPatch<TESS_TERRAIN_VS_OUT, NUM_CONTROL_POINTS> ip)
{
	TESS_TERRAIN_HS_DATA_OUT Output = (TESS_TERRAIN_HS_DATA_OUT)0;

	Output.inside[0] = 1;
	Output.inside[1] = 1;
	Output.edges[0] =  1;
	Output.edges[1] =  1;
	Output.edges[2] =  1;
	Output.edges[3] =  1;

	// SOLUTION_BEGIN
	// Frustum culling will act on a sphere based on the size of the patch we are testing.
	const float3 center = 0.25 * (ip[0].position + ip[1].position + ip[2].position + ip[3].position);
	
	const float  diameter = length(ip[0].position.xyz-ip[2].position.xyz); 

	// Test if the patch is not in the frustum
	if (0 == inFrustum(center, diameter))
	{
		// Setting tesselation factors to -1 removes the patch from further stages of the pipeline
		Output.inside[0] = -1;
		Output.inside[1] = -1;
		Output.edges[0] = -1;
		Output.edges[1] = -1;
		Output.edges[2] = -1;
		Output.edges[3] = -1;
		return Output;
	}

	// Set our XZ position variables that will be used by later stages to create texture coordinates
	Output.worldXZ[0] = ip[0].worldXZ;
	Output.worldXZ[1] = ip[1].worldXZ;
	Output.worldXZ[2] = ip[2].worldXZ;
	Output.worldXZ[3] = ip[3].worldXZ;

	// Tesselation factors will be based on projected size of the edge of a tile. Tiles are assumed
	// to be square. If a tile borders a different sized tile, the tesselation factors will
	// be set for the pair by neighboring powers of two, this prevent T junctions without special geometry
	float small = 8;
	float large = 16;

	//Output.edges[0] = SphereToScreenSpaceTessellation(ip[0].position, ip[1].position, gTileSize);
	//Output.edges[2] = SphereToScreenSpaceTessellation(ip[2].position, ip[3].position, gTileSize);
	//Output.edges[1] = SphereToScreenSpaceTessellation(ip[3].position, ip[0].position, gTileSize);
	//Output.edges[3] = SphereToScreenSpaceTessellation(ip[1].position, ip[2].position, gTileSize);

	// Is right larger
	if (ip[0].posXScale > 1)
		Output.edges[0] = small;
	// Is right smaller
	else if (ip[0].posXScale < 1)
		Output.edges[0] = large;
	else
		Output.edges[0] = SphereToScreenSpaceTessellation(ip[0].position, ip[1].position, gTileSize);

	// is left smaller
	if (ip[0].negXScale < 1)
		Output.edges[2] = large;
	// is leOutputft larger
	else if (ip[0].negXScale > 1)
		Output.edges[2] = small;
	else
		Output.edges[2] = SphereToScreenSpaceTessellation(ip[2].position, ip[3].position, gTileSize);

	// Is top larger
	if (ip[0].posYScale > 1)
		Output.edges[1] = small;
	// Is top smaller
	else if (ip[0].posYScale < 1)
		Output.edges[1] = large;
	else
		Output.edges[1] = SphereToScreenSpaceTessellation(ip[3].position, ip[0].position, gTileSize);

	// Is bottom smaller
	if (ip[0].negYScale < 1)
		Output.edges[3] = large;
	// Is bottom larger
	else if (ip[0].negYScale > 1)
		Output.edges[3] = small;
	else
		Output.edges[3] = SphereToScreenSpaceTessellation(ip[1].position, ip[2].position, gTileSize);

	// Use average of edge points for interior - visually looks OK.  
	Output.inside[1] = 0.5 * (Output.edges[0] + Output.edges[2]);
	Output.inside[0] = 0.5 * (Output.edges[1] + Output.edges[3]);

	// SOLUTION_END

	// Debug colors show the tesselation factors of a primitive. The below code will map
	// the tesselation factors to 0-5 based on their nearest power of 2 ranging 2-64
	Output.debugColor[0] = DEBUG_COLORS[clamp(log2(Output.edges[0]), 0, 5)];
	Output.debugColor[1] = DEBUG_COLORS[clamp(log2(Output.edges[1]), 0, 5)];
	Output.debugColor[2] = DEBUG_COLORS[clamp(log2(Output.edges[2]), 0, 5)];
	Output.debugColor[3] = DEBUG_COLORS[clamp(log2(Output.edges[3]), 0, 5)];
	Output.debugColor[4] = DEBUG_COLORS[clamp(log2(Output.inside[0]), 0, 5)];
	return Output;
}


[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
TESS_TERRAIN_HS_OUT main( InputPatch<TESS_TERRAIN_VS_OUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID)
{
	TESS_TERRAIN_HS_OUT Output;

	// This is just a pass through method
	// Tesselation factors are calculated in the patch-constant method
	Output.position = ip[i].position;

	return Output;
}

#undef NUM_CONTROL_POINTS
#endif //GBUFFERS_TESSELLATED_TERRAIN_HS
