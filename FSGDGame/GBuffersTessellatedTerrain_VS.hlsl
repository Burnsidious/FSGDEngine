#ifndef GBUFFERS_TESSELLATED_TERRAIN_VS_HLSL
#define GBUFFERS_TESSELLATED_TERRAIN_VS_HLSL

#include "VertexLayouts.hlsli"
#include "TerrainSupport.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

// For debug use
static const float SHOW_TILES_SCALE = 0.98;

TESS_TERRAIN_VS_OUT main( VERTEX_TERRAIN input )
{
	TESS_TERRAIN_VS_OUT output;

	// Create real vertex position from instace data

	// Each "tile" is rendered once for each vertex.
	// The input position recived is the center point of the tile.
	// The vertexID value is created by the IA stage, but will match the attached index buffer order.
	// The vertexID, coupled with a known even displacement of patches is all that is needed to 
	// rebuild correct vertices

	// Which vert accross the edge of a tile are we dealing with, assuming 2 verts per tile edge
	float recipVertsPerTileEdge = 1.0f / 2.0f;
	float zIndex = floor(input.vertexId * recipVertsPerTileEdge);
	float xIndex = input.vertexId - zIndex * 2.0f;

	// When gShowTiles is not equal to zero, scaling the size value by less than one
	// will create a gap inbetween tiles which can be useful for debugging
	float size = gTileSize;
	if(gShowTiles)
		size *= SHOW_TILES_SCALE;

	// Scale relative positions by size of the tile, and offset position by tile position and camera
	// The gTerrainCameraPosOffsetXZ value is the camera position, but truncated to a defined
	// step size. This causes tile movement to be chunky instead of constant, which reduces the swimming of pixels
	output.position = float3(xIndex * size + input.position.x + gTerrainCameraPosOffsetXZ.x, 0,
		zIndex * size + input.position.y + gTerrainCameraPosOffsetXZ.y);

	// The patches created will later be tessellated, and displaced by the height map but
	// we need to get appropriate heights for patch vertices to get correct frustum culling later
	output.position.y = CalculateTerrainHeight(output.position.xz);

	// pass through the rest

	// These world position values will be passed to pixel shader 
	// to be used to create texture coordinates
	output.worldXZ = output.position.xz;
	
	// These values are used to adjust tesselation on borders of different sized tiles
	// without cracks
	output.negXScale = input.negXScale;
	output.negYScale = input.negYScale;
	output.posXScale = input.posXScale;
	output.posYScale = input.posYScale;
	
	return output;
}

#endif //GBUFFERS_TESSELLATED_TERRAIN_VS_HLSL
