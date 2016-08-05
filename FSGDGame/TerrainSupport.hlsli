#ifndef TERRAIN_SUPPORT_HLSLI
#define TERRAIN_SUPPORT_HLSLI

#include "../EDRendererD3D/ConstantBuffers.h"
#include "TexturesAndSamplers.hlsli"

static const float3 DEBUG_COLORS[6] = 
{
	float3(0,0,1),  //  2 - blue
	float3(0,1,1),	//  4 - cyan
	float3(0,1,0),	//  8 - green
	float3(1,1,0),	// 16 - yellow
	float3(1,0,1),	// 32 - purple
	float3(1,0,0),	// 64 - red
};

float2 CalculateTerrainHeightTexCoord(float2 worldXZ)
{
	return worldXZ / gTerrainHeightCoordFactor;
}

float2 CalculateTerrainDiffuseTexCoord(float2 worldXZ)
{
	return worldXZ / gTerrainDiffuseCoordFactor;
}

float CalculateTerrainHeight(float2 worldXZ)
{
	//return -1;
	float2 texCoord = CalculateTerrainHeightTexCoord(worldXZ);

	//return gTerrainHeightScale *
		//(courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0).a + gTerrainHeightOffset);

	int blurScale = 7;

	float height = dot(float3(

	// Average the first three values
	dot(
	float3(
	courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0, int2(0, 0)).a,
	courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0, int2(blurScale, 0)).a,
	courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0, int2(-blurScale, 0)).a), 1.0 / 3.0),

	// Average the second three values
	dot(
	float3(
	courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0, int2(0, blurScale)).a,
	courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0, int2(0, -blurScale)).a,
	courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0, int2(blurScale, blurScale)).a), 1.0 / 3.0),

	// Average the last three values
	dot(
	float3(
	courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0, int2(-blurScale, -blurScale)).a,
	courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0, int2(-blurScale, blurScale)).a,
	courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0, int2(blurScale, -blurScale)).a), 1.0 / 3.0)),

	1.0 / 3.0);

	return gTerrainHeightScale *
		(height + gTerrainHeightOffset);
	return gTerrainHeightScale * 
		(courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0).a + gTerrainHeightOffset);
	//(courseHeightMap.SampleLevel(linearWrapSampler, texCoord, 0).r + gTerrainHeightOffset);
}

float3 bilerpColor(float3 c0, float3 c1, float3 c2, float3 c3, float2 UV)
{
	float3 left = lerp(c0, c1, UV.y);
		float3 right = lerp(c2, c3, UV.y);
		float3 result = lerp(left, right, UV.x);
		return result;
}

float3 lerpDebugColors(float3 cIn[5], float2 uv)
{
	if (uv.x < 0.5 && uv.y < 0.5)
		return bilerpColor(0.5* (cIn[0] + cIn[1]), cIn[0], cIn[1], cIn[4], 2 * uv);
	else if (uv.x < 0.5 && uv.y >= 0.5)
		return bilerpColor(cIn[0], 0.5* (cIn[0] + cIn[3]), cIn[4], cIn[3], 2 * (uv - float2(0, 0.5)));
	else if (uv.x >= 0.5 && uv.y < 0.5)
		return bilerpColor(cIn[1], cIn[4], 0.5* (cIn[2] + cIn[1]), cIn[2], 2 * (uv - float2(0.5, 0)));
	else // x >= 0.5 && y >= 0.5
		return bilerpColor(cIn[4], cIn[3], cIn[2], 0.5* (cIn[2] + cIn[3]), 2 * (uv - float2(0.5, 0.5)));
}

#endif //TERRAIN_SUPPORT_HLSLI
