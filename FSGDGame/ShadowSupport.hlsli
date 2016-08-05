#ifndef SHADOW_SUPPORT_HLSLI
#define SHADOW_SUPPORT_HLSLI

#include "LightSupport.hlsli"
#include "TexturesAndSamplers.hlsli"

#define ShadowNormOffset 0.025f

float CalculatePointLightShadow(float3 notNormalizedToLight, float toLightLength )
{
	// SOLUTION_BEGIN
	
	// Calculate the linear depth of this fragment from the light source
	float sDepth = max(max(abs(notNormalizedToLight.x), abs(notNormalizedToLight.y)), 
		abs(notNormalizedToLight.z));

	// Normalize toLight before using as shadow map texture coordinates
	float3 toLight = notNormalizedToLight / toLightLength;

	// Map the linear depth of this frgament to the the logrithmic range stored in the shadow map
	sDepth = pointLightShadowData.planeY / sDepth - pointLightShadowData.planeX;

	// Adjust depth based on depth bias to prevent self-shadowing surface acne
	float depthComparisonValue = sDepth - pointLightShadowData.shadowBias;

	float shadow = point0ShadowMap.SampleCmpLevelZero(linearBorderCompareSampler, -toLight, 
		depthComparisonValue);

	// The previous method does the equivalent of the following code, but blends all samples
	// giving a much smoother gradient of the penumbra of our shadows. 
	
	//lightMapDepth = point0ShadowMap.Sample(linearClampSampler, texCubeCoords).x;
	//shadow = 1.0f * (sDepth - gShadowDepthOffset < (lightMapDepth));

	return shadow;
	// SOLUTION_END
	return 1;
}

// Tests if a given location is in shadow
// shadowMap - the shadow map to test against
// texCoord - The central coordinate in the shadow map representing this location
// depth - The depth of the fragment we are testing whether is in shadow
// (optional) texCoordOffset - An offset, in pixels, to test around texCoord
// returns A value between 0 and 1, where 0 represents a location completely in shadow.  
// Shadow comparisons use the SampleCmpLevelZero method which will blur multiple samples, 
// resulting in softer edges on shadows
float CalculateShadow(Texture2D shadowMap, float2 texCoord, 
					  float depth, int2 texCoordOffset = float2(0, 0))
{
	float shadow = 1;
	// SOLUTION_BEGIN
	// Reduce our depth compare value by our shadow bias to reduce self-shadowing surface acne
	float depthComparisonValue = depth - gShadowDepthOffset;

	shadow = shadowMap.SampleCmpLevelZero(linearBorderCompareSampler, texCoord, 
		depthComparisonValue, texCoordOffset);

	// The previous method does the equivalent of the following code, but blends all samples
	// giving a much smoother gradient of the penumbra of our shadows. This is required for 
	// percentage closer filtering
	
	//float lightMapDepth = shadowMap.Sample(linearWrapSampler, texCoord, texCoordOffset).x;
	//shadow = 1.0f * (depth - gShadowDepthOffset < lightMapDepth);

	// SOLUTION_END
	return shadow;
}

// Tests if a given location is in shadow, using a 3x3 kernal to average neighboring shadow 
// values resulting in softer smoother shadow penumbra
// shadowMap - the shadow map to test against
// texCoord - The central coordinate in the shadow map representing this location
// depth - The depth of the fragment we are testing whether is in shadow
float CalculateShadowPCF(Texture2D shadowMap, float2 texCoord, float depth)
{
	//return CalculateShadow(shadowMap, texCoord, depth);
	// SOLUTION_BEGIN

	//return (  CalculateShadow(shadowMap, texCoord, depth, int2(0, 0)) 
	//		+ CalculateShadow(shadowMap, texCoord, depth, int2(1, 0))
	//		+ CalculateShadow(shadowMap, texCoord, depth, int2(-1, 0))
	//		+ CalculateShadow(shadowMap, texCoord, depth, int2(0, 1))
	//		+ CalculateShadow(shadowMap, texCoord, depth, int2(0, -1))
	//		+ CalculateShadow(shadowMap, texCoord, depth, int2(1, 1))
	//		+ CalculateShadow(shadowMap, texCoord, depth, int2(-1, -1))
	//		+ CalculateShadow(shadowMap, texCoord, depth, int2(-1, 1))
	//		+ CalculateShadow(shadowMap, texCoord, depth, int2(1, -1))) / 9;

	// This monstrosity is the equivalent of the easier to read commented out code above.
	// do not expect students to implement their solution this way
	return (
		// Average the results the 3 other averages to find the total average shadow coverage for 
		// the current fragemnt
		dot(float3(

		// Average the first three shadow values
		dot(
		float3(CalculateShadow(shadowMap, texCoord, depth, int2(0, 0)), 
		CalculateShadow(shadowMap, texCoord, depth, int2(1, 0)),
		CalculateShadow(shadowMap, texCoord, depth, int2(-1, 0))), 1.0/3.0), 

		// Average the second three shadow values
		dot(
		float3(CalculateShadow(shadowMap, texCoord, depth, int2(0, 1)),
		CalculateShadow(shadowMap, texCoord, depth, int2(0, -1)),
		CalculateShadow(shadowMap, texCoord, depth, int2(1, 1))), 1.0 /3.0),

		// Average the last three shadow values
		dot(
		float3(CalculateShadow(shadowMap, texCoord, depth, int2(-1, -1)),
		CalculateShadow(shadowMap, texCoord, depth, int2(-1, 1)),
		CalculateShadow(shadowMap, texCoord, depth, int2(1, -1))),  1.0/3.0)), 

		1.0/3.0) );
	// SOLUTION_END
	return 0;
}

#endif //SHADOW_SUPPORT_HLSLI