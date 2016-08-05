#ifndef DEFERRED_DIR_LIGHT_HLSL
#define DEFERRED_DIR_LIGHT_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "LightSupport.hlsli"
#include "ShadowSupport.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"
#include "../EDRendererD3D/LightBuffers.h"

float4 main(TwoDVertexWithPositionOut inVert) : SV_TARGET0
{
	//return 0;
	float4 specular, posWorld, lightSpacePos, diffuse,
	finalAmbient, finalDiffuse, finalSpecular;
	float3 normal, reflectionVector, directionToCamera;
	float2 texCoord;
	float depth, lightMapDepth, shadow, offset, nDotL, specMod;

	// SOLUTION_BEGIN
	texCoord = inVert.texCoord;

	// Sample GBuffer data
	diffuse = diffuseGBuffer.Sample(pointClampSampler, texCoord);
	normal = normalGBuffer.Sample(pointClampSampler, texCoord).rgb;
	specular = specularGBuffer.Sample(pointClampSampler, texCoord);
	depth = depthGBuffer.Sample(pointClampSampler, texCoord).r;

	// convert normal-color (0 -> +1) to normal range (-1 -> +1)
	normal = normal * 2 - 1;

	// Calculate fragment's view space position
	posWorld = CalculateWorldSpacePosition(inVert.pixelPosition.xy, depth, gInvViewProj);

	///////////////////////////////////////////////
	// Shadows

	//// Calculate fragment's light space position
	//lightSpacePos = mul(posWorld, DirLight.viewProjection[0]);
	//// Convert to Clip space range ( -1 -> +1)
	//lightSpacePos.xyz = lightSpacePos.xyz / lightSpacePos.w;
	//// Convert to texture coordinate range (0 -> +1)
	//lightSpacePos.xy = (lightSpacePos.xy * 0.5) + 0.5;
	//// Texture space is flipped alone the Y axis
	//lightSpacePos.y = 1 - lightSpacePos.y;

	//// Averaging the neighboring blended shadow results gives us Percentage closer filtering
	//// The current implementation models a 3x3 kernal. The code could be changed to use a nested
	//// loop and variable kernal sizes resulting in higher amounts of filtering.
	//// In practice, kernals larger than 3x3 give poor aesthetic results and loops increase the rendering cost.
	//shadow = CalculateShadowPCF(directional0ShadowMap[0], lightSpacePos.xy, lightSpacePos.z);

	const float minShadowTex = 0.01;
	const float maxShadowTex = 0.99;
	shadow = 1.0f;
	unsigned int index = 0;
	//unsigned int index = 2;
	for (; index < 3; ++index)
	{
		// Calculate fragment's light space position
		lightSpacePos = mul(posWorld, DirLight.viewProjection[index]);
		// Convert to Clip space range ( -1 -> +1)
		lightSpacePos.xyz = lightSpacePos.xyz / lightSpacePos.w;
		// Convert to texture coordinate range (0 -> +1)
		lightSpacePos.xy = (lightSpacePos.xy * 0.5) + 0.5;

		if (min(min(lightSpacePos.x, lightSpacePos.y), lightSpacePos.z) > minShadowTex &&
			max(max(lightSpacePos.x, lightSpacePos.y), lightSpacePos.z) < maxShadowTex)
		{
			//cascadeIndex = index;
			lightSpacePos.y = 1 - lightSpacePos.y;

			// Averaging the neighboring blended shadow results gives us Percentage closer filtering
			// The current implementation models a 3x3 kernal. The code could be changed to use a nested
			// loop and variable kernal sizes resulting in higher amounts of filtering.
			// In practice, kernals larger than 3x3 give poor aesthetic results and loops increase the rendering cost.

			// Forces the loop to unroll which results in many more arithmetic slots used
			//shadow = CalculateShadowPCF(directional0ShadowMap[index], lightSpacePos.xy, lightSpacePos.z);

			if (0 == index)
				shadow = CalculateShadowPCF(directional0ShadowMap[0], lightSpacePos.xy, lightSpacePos.z);
			else if (1 == index)
				shadow = CalculateShadowPCF(directional0ShadowMap[1], lightSpacePos.xy, lightSpacePos.z);
			else
				shadow = CalculateShadowPCF(directional0ShadowMap[2], lightSpacePos.xy, lightSpacePos.z);
			break;
		}
	}

	///////////////////////////////////////////////
	// End Shadows

	// Calculate angle based light intensity
	nDotL = saturate(dot(normal, -DirLight.direction));

	// Specular
	specMod = CalculateSpecularAmount(-DirLight.direction, normal, posWorld.xyz, 
		DirLight.specularIntensity, DirLight.specularPower);

	// Calculate final color components
	finalAmbient = float4(DirLight.ambientColor, 1) * diffuse * diffuse.a;
	finalDiffuse = diffuse * nDotL * float4(DirLight.diffuseColor, 1);
	finalSpecular = specular * specMod * (nDotL > 0) * float4(DirLight.specularColor, 1);

	float4 finalColor = finalAmbient + shadow * (finalDiffuse + finalSpecular);
	return finalColor;
	// SOLUTION_END

	return 0;
}
#endif //DEFERRED_DIR_LIGHT_HLSL