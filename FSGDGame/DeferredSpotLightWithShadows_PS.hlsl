#ifndef DEFERRED_SPOT_LIGHT_WITH_SHADOWS_HLSL
#define DEFERRED_SPOT_LIGHT_WITH_SHADOWS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "LightSupport.hlsli"
#include "ShadowSupport.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"
#include "../EDRendererD3D/LightBuffers.h"

float4 main(VertexWithPositionOut inVert) : SV_TARGET0 
{
	float4 diffuse, specular, posWorld, 
		lightSpacePos, finalAmbient, finalDiffuse, finalSpecular;
	float3 toLight, texCubeCoords, normal;
	float2 texCoord;
	float toLightLength, attenuation = 0, depth, nDotL, specMod,
		spotEffect, shadow;

	// SOLUTION_BEGIN
	// convert to clip space
	inVert.pixelPosition.xy /= inVert.pixelPosition.w;

	// Use clip space position for texcoords
	texCoord = 
		float2(inVert.pixelPosition.x, -inVert.pixelPosition.y) * 0.5f + 0.5f;

	// Align texels to pixels
    //texCoord += gHalfPixel;

	diffuse = diffuseGBuffer.Sample(linearClampSampler, texCoord);
	normal = normalGBuffer.Sample(pointClampSampler, texCoord).rgb;
	depth = depthGBuffer.Sample(linearClampSampler, texCoord).r;
	specular = specularGBuffer.Sample(linearClampSampler, texCoord);

	// Put the normal in clip space
	normal = normal * 2 - 1;

	// Get screen space position
	posWorld = CalculateWorldSpacePosition(inVert.pixelPosition.xy, depth, gInvViewProj);

	toLight = spotLight.position.xyz - posWorld.xyz;
	toLightLength = length(toLight);

	toLight /= toLightLength;

	finalAmbient = finalDiffuse = finalSpecular = 0;
	shadow = 1.0f;

	nDotL = saturate(dot(toLight, normal.xyz));

	if(nDotL > 0)
	{		
		// Spot ~angle
		spotEffect = dot(spotLight.direction.xyz, -toLight);

		if(spotEffect > spotLight.cutoff)
		{
            spotEffect = pow(abs(spotEffect), abs(spotLight.exponent));

			//attenuate...
			attenuation = CalculateAttenuation(spotLight.attenuation, toLightLength, 
				spotLight.range, spotEffect);

			// Specular
			specMod = CalculateSpecularAmount(toLight, normal, posWorld.xyz, 
				spotLight.specularIntensity, spotLight.specularPower);
	
			finalAmbient = float4(spotLight.ambientColor, 1) * diffuse * diffuse.a;
			finalDiffuse = diffuse * nDotL * float4(spotLight.diffuseColor, 1);
			finalSpecular = specular * specMod * (nDotL > 0) * float4(spotLight.specularColor, 1);

			// Shadows
			lightSpacePos = mul(posWorld, spotLightShadowData.viewProjection);
            lightSpacePos.xyz = lightSpacePos.xyz/lightSpacePos.w;
            lightSpacePos.xy = (lightSpacePos.xy * 0.5) + 0.5;
            lightSpacePos.y = 1 - lightSpacePos.y;

			shadow = CalculateShadow(spot0ShadowMap, lightSpacePos.xy, lightSpacePos.z);
		}

	}

	float4 finalColor = attenuation * (finalAmbient + ( shadow * (finalDiffuse + finalSpecular)));
	//if(dot(finalColor, 1) == 0) discard;
	return finalColor;
	// SOLUTION_END
	return 0;
}
#endif //DEFERRED_SPOT_LIGHT_WITH_SHADOWS_HLSL
