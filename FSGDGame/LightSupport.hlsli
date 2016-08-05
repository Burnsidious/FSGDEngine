#ifndef LIGHT_SUPPORT_HLSLI
#define LIGHT_SUPPORT_HLSLI

#include "../EDRendererD3D/ConstantBuffers.h"

float CalculateAttenuation(float3 attenuationFactors, float distanceToLight, float lightRange, 
						   float numerator = 1.0)
{
	// SOLUTION_BEGIN

	float attenuation = saturate(numerator / (attenuationFactors.x +
		attenuationFactors.y * distanceToLight +
		attenuationFactors.z * distanceToLight * distanceToLight));

	float distRatio = distanceToLight / lightRange;
	float damping = saturate(1 - pow(distRatio, 2));
	attenuation *= damping;

	return attenuation;
	
	// SOLUTION_END
	return 0;
}

float4 CalculateWorldSpacePosition(float2 pixelPosition, float pixelDepth, 
								   float4x4 inverseViewProjection)
{
	// SOLUTION_BEGIN
	float4 posScreen, posWorld;

	posScreen.xy = pixelPosition.xy;
	posScreen.z = pixelDepth;
	posScreen.w = 1;

	// Calculate fragment's world space position
	posWorld = mul(posScreen, inverseViewProjection);
	posWorld /= posWorld.w;

	return posWorld;
	// SOLUTION_END
	return 0;
}

// Calculates how bright of a specualr highlight/reflection a given location should have
// normalizedToLight - a direction vector pointing to the light
// normal - a direction vector representing the orientation of the surface
// worldSpaceFragPos - The location of the fragment in world space
// specularLightIntensisty - The lights specular intensisty
// specularLightPower - The lights specular power
float CalculateSpecularAmount(float3 normalizedToLight, float3 normal, float3 worldSpaceFragPos,
							  float specularLightIntensisty, float specularLightPower)
{
	// SOLUTION_BEGIN

	// Specular
	float3 reflectionVector = (reflect(-normalizedToLight, normal));
	//camera-to-surface vector
	float3 directionToCamera = normalize(gCameraPos - worldSpaceFragPos);
	//compute specular light
	return saturate(specularLightIntensisty * pow( saturate(
		dot(reflectionVector, directionToCamera)), specularLightPower));
	// SOLUTION_END
	return 0;
}
#endif //LIGHT_SUPPORT_HLSLI
