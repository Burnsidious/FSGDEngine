#ifndef CONSTANT_BUFFERS_H
#define CONSTANT_BUFFERS_H

#include "SharedDefines.h"

// Starting with register 3 as the first three buffers are used 
// for directionalights, point lights and spot lights.
CONSTANT_BUFFER_BEGIN(cbPerObject, b0)
	/// The concatenated world view projection matrices
	float4x4	gMVP;
	/// The world matrix of the object to be rendered.
	float4x4	gWorld;
#ifdef __cplusplus
	const static int REGISTER_SLOT = 0;
#endif
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbPerCamera, b1)
	/// The current cameras current view projection concatenated matrix
	float4x4	gViewProj;
	/// The current cameras current inverse view-projection matrix
	float4x4	gInvViewProj;

	/// The position of the camera, or eye.
	float3 gCameraPos;
	/// The distance of the far clip plane
	float gFarDist;
	// The size of the main render target in pixels
	float2 gScreenSize;
	/// Precomputed value used to convert depth to actual distance.
	/// gPlaneX = -far / (far - near)
	/// distance = gPlaneY / (gPlaneX + depth)
	float gPlaneX;
	/// Precomputed value used to convert depth to actual distance.
	/// gPlaneY = -far * near/ (far - near)
	/// distance = gPlaneY / (gPlaneX + depth)
	float gPlaneY;
	float3 gCameraDir;

#ifdef __cplusplus
	const static int REGISTER_SLOT = 1;
#endif
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbMisc, b2)
	/// The difference between texel and pixel
	float2 gHalfPixel;
	/// An offset to be used when comparing depths for shadow calculations
	float gShadowDepthOffset;
	/// An offset to be used to find the texels in the directional light shadow 
	/// map around the one being sampled.
	float gShadowDirOffset;

	float gDisplacementScale;

	float gMaxTessDistance;
	float gMinTessDistance;
	float gMaxTessFactor;
	float gMinTessFactor;
#ifdef __cplusplus
	const static int REGISTER_SLOT = 2;
#endif
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbDebug, b3)
	float gLineWidth;
#ifdef __cplusplus
	const static int REGISTER_SLOT = 3;
#endif
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbPostProcess, b6)
	float gTexCoordScale;
	float gFocusDistance;
	float gFocusRange;
	float gEdgeWidth;
	float gEdgeIntensity;
	float gTheta;
	float gScale;
	float gFade;
#ifdef __cplusplus
	const static int REGISTER_SLOT = 6;
#endif
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbPerSkinnedObject, b0)
#ifdef __cplusplus
	const static int REGISTER_SLOT = 0;
#endif	
	const static int NUM_BONES = 50;
	//float4x4	gSkinnedWorld;

	float4x4	gSkinnedViewProj;

	float4x4	gSkinnedMatrices[NUM_BONES];
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbPerTileFrame, b0)
#ifdef __cplusplus
	const static int REGISTER_SLOT = 0;
#endif	

	/// The world matrix of the object to be rendered.
	float4x4	gTileWorldView;
	float4x4	gTileProj;
	float4x4	gTileMVP;
	float		gTileSize;
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbTerrain, b4)
#ifdef __cplusplus
	const static int REGISTER_SLOT = 4;
#endif	
	//gTessellatedTriWidth is desired pixels per tri edge
	float gTessellatedTriWidth;
	// How many verts there should be along the edge of a tile, before tessellation
	//float gVertsPerTileEdge;
	//float gPatchesPerTileEdge;
	// Texture coordinates for terrain are based off of X-Z positions. This controls the scaling
	// of the coordinates used for sampling the height map. TexCoord = worldXZ / gTerrainHeightCoordFactor
	float gTerrainHeightCoordFactor;

	float gTerrainDiffuseCoordFactor;
	// Sampled heights range 0 to 1, then our scaled by this factor for final height calculations.
	float gTerrainHeightScale;
	float2 gTerrainCameraPosOffsetXZ;

	// A value added to sampled height map values before being scaled
	// If half the range of height map values are above "sea level" this value should be -0.5;
	float gTerrainHeightOffset;
	// Debug values
	int gShowTiles;
	int gShowTerrainTransistions;

	int gTerrainTest;
CONSTANT_BUFFER_END

#endif //CONSTANT_BUFFERS_H
