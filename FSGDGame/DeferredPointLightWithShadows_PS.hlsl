#ifndef DEFERRED_POINT_LIGHT_WITH_SHADOWS_HLSL
#define DEFERRED_POINT_LIGHT_WITH_SHADOWS_HLSL

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "LightSupport.hlsli"
#include "ShadowSupport.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"
#include "../EDRendererD3D/LightBuffers.h"

float4 main(VertexWithPositionOut inVert) : SV_TARGET0 
{
	float4 diffuse, specular, posWorld, finalAmbient, finalDiffuse, finalSpecular;
	float3 toLight, normal;
	float2 texCoord;
	float toLightLength, attenuation, nDotL, lightMapDepth, shadow, depth, specMod;

	// SOLUTION_BEGIN
	// convert to clip space
	inVert.pixelPosition.xy /= inVert.pixelPosition.w;

	// Use clip space position for texcoords
	texCoord = 
		float2(inVert.pixelPosition.x, -inVert.pixelPosition.y) * 0.5f + 0.5f;

	// Align texels to pixels
	diffuse = diffuseGBuffer.Sample(linearClampSampler, texCoord);
	normal = normalGBuffer.Sample(pointClampSampler, texCoord).rgb;
	depth = depthGBuffer.Sample(linearClampSampler, texCoord).r;
	specular = specularGBuffer.Sample(linearClampSampler, texCoord);

	// Put the normal in clip space
	normal = normal * 2 - 1;

	// Get screen space position
	posWorld = CalculateWorldSpacePosition(inVert.pixelPosition.xy, depth, gInvViewProj);

	toLight = pointLight.position.xyz - posWorld.xyz;
	toLightLength = length(toLight);

	//attenuate...
	attenuation = CalculateAttenuation(pointLight.attenuation, toLightLength, pointLight.range);

	// Calculate shadow amount
	shadow = CalculatePointLightShadow(toLight, toLightLength);

	toLight /= toLightLength;
	nDotL = saturate(dot(toLight, normal.xyz));

	// Specular
   	specMod = CalculateSpecularAmount(toLight, normal, posWorld.xyz, 
		pointLight.specularIntensity, pointLight.specularPower);
	
	finalAmbient = float4(pointLight.ambientColor, 1) * diffuse * diffuse.a;
	finalDiffuse = diffuse * nDotL * float4(pointLight.diffuseColor, 1);
	finalSpecular = specular * specMod * (nDotL > 0) * float4(pointLight.specularColor, 1);

	float4 finalColor = attenuation * (finalAmbient + ( shadow * (finalDiffuse + finalSpecular)));
	//'if(dot(finalColor, 1) == 0) discard;
	return finalColor;
	// SOLUTION_END
	return 0;
}
#endif //DEFERRED_POINT_LIGHT_WITH_SHADOWS_HLSL