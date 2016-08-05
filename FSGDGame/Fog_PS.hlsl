#ifndef FOG_PS_HLSL
#define FOG_PS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"
#include "../EDRendererD3D/LightBuffers.h"

static const float density = 0.002f;

float4 main(TwoDVertexOut input) : SV_TARGET
{
	float4 sceneColor, posScreen, posWorld;
	float3 fogColor;
	float depth, distanceToFrag, fogScale;

	fogColor = float3(0.351f, .464f, .558f);
	//lightColor = float3(1, 1, 1);

	depth = depthGBuffer.Sample(pointClampSampler, input.texCoord).r;
	sceneColor = postSourceMap.Sample(linearClampSampler, input.texCoord);

	posScreen.x = input.texCoord.x * 2 - 1;
	posScreen.y = -(input.texCoord.y * 2 - 1);
	posScreen.z = depth;
	posScreen.w = 1;

	posWorld = mul(posScreen, gInvViewProj);
	posWorld /= posWorld.w;

	float3 camToFrag = posWorld.xyz - gCameraPos;
	distanceToFrag = length(camToFrag);
	camToFrag /= distanceToFrag;
	// standard
	//fogScale = saturate(distanceToFrag/gFarDist);
	
	// exponential
	//fogScale = exp(-distanceToFrag * density);
	//fogScale = saturate(1 - fogScale);

	// exponential squared
	//fogScale = exp(-pow( distanceToFrag * density,4));
	//fogScale = saturate(1 - fogScale);

	// Light modulated
	//fogScale = 1.0f - exp(-distanceToFrag * density);
	//fogScale = exp(-pow( distanceToFrag * density, 4));
	//fogScale = saturate(1 - fogScale);

	fogScale = 1 * exp(-gCameraPos.y * density) *
		(1.0 - exp(-distanceToFrag*camToFrag.y * density)) / camToFrag.y;

	float sunAmount = max(dot(camToFrag, -DirLight.direction), 0.0);

	fogColor = lerp(fogColor, DirLight.diffuseColor, pow(sunAmount, 8.0));

	//return float4(DirLight.diffuseColor, 1);
	//return float4(lerp(sceneColor.xyz, DirLight.diffuseColor, fogScale), 1);
	return float4(lerp(sceneColor.xyz, fogColor, saturate(fogScale)), 1);

}

#endif //FOG_PS_HLSL
