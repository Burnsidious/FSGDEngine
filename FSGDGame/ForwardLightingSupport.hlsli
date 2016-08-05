#ifndef FORWARD_LIGHTING_SUPPORT_HLSLI
#define FORWARD_LIGHTING_SUPPORT_HLSLI

#include "../EDRendererD3D/LightBuffers.h"
#include "TexturesAndSamplers.hlsli"
#include "LightSupport.hlsli"

/// A method used to find the color after lighting from a directional light.
/// \param dirLight - The DirectionalLight to light this surface
/// \param halfVec - A vector half way between the light and camera
/// \param normal - The normal of the surface to light
/// \param matColor - The color of the surface before lighting
float4 ApplyDirectionalLight(DirectionalLightCBuffer dirLight, float3 directionToCamera, 
	float3 fragmentPositonW, float4 normal, float4 matColor)
{
	float4 diffuse, ambient, specular, color;
	float nDotL, specMod;

	color = float4(0, 0, 0, 0);
    if(dirLight.enabled)
	{
		nDotL = saturate(dot(-dirLight.direction, normal.xyz));
		diffuse = matColor * float4(dirLight.diffuseColor, 1);
		specular = float4(dirLight.specularColor, 1);
		ambient = matColor * float4(dirLight.ambientColor, 1);

		specMod = CalculateSpecularAmount(-dirLight.direction, normal.xyz, fragmentPositonW,
			dirLight.specularIntensity, dirLight.specularPower);

		color.xyz = (nDotL * diffuse.xyz + ambient.xyz + specMod * specular.xyz);
		color.a = specMod + diffuse.a;
	}
	return color;
}

float4 ApplyPointLight(PointLightCBuffer inPointLight, float3 directionToCamera, float3 fragmentPositon,
					   float3 normal, float4 fragmentColor, float specularMaterial)
{
	float4 color = float4(0, 0, 0, 0);
    if(inPointLight.enabled)
	{
		float4 diffuse = fragmentColor * float4(inPointLight.diffuseColor, 1);
		float4 ambient = fragmentColor * float4(inPointLight.ambientColor, 1);
		float4 specular = specularMaterial * float4(inPointLight.specularColor, 1);

		float3 toLight = inPointLight.position.xyz - fragmentPositon;
		float toLightLength = length(toLight);
		toLight/=toLightLength;

		float nDotL = saturate(dot(toLight, normal));
		
		if(nDotL > 0.0)
		{
			float attenuation = CalculateAttenuation(inPointLight.attenuation, toLightLength, 
				inPointLight.range);
			// Specular 
			float specMod = CalculateSpecularAmount(toLight, normal, fragmentPositon, 
				inPointLight.specularIntensity, inPointLight.specularPower);

			color.xyz = attenuation * (nDotL * diffuse.xyz + ambient.xyz + specMod * specular.xyz);
			color.a = attenuation * specMod + diffuse.a;
		}
	}

	return color;
}

/// A method used to find the color after lighting from a point light, with 
/// shadow mapping.
/// \param pointLight - The PointLight to light this surface
/// \param halfVec - A vector half way between the light and camera
/// \param dirDistance - x,y,z should be the normalized light direction, 
/// w should be distance to the light
/// \param normal - The normal of the surface to light
/// \param ambientMatColor - The ambient color of the surface material
/// \param diffuseMatColor - The diffuse color of the surface material
/// \param specularMatColor - The specular color of the surface material
/// \return Returns the resulting color of the surface after applying the 
/// point light
//float4 ApplyPointLightWithShadowsPS(PointLightWithShadowCBuffer pointLight, float3 directionToCamera,
//									float3 halfVec, float4 dirDistance, float4 normal, 
//									float4 ambientMatColor, float4 diffuseMatColor, 
//									float4 specularMatColor)
//{
//	float pointDepth, depth;
//	float4 pointColor;
//	float3 texPointCoords;
//	
//	texPointCoords = -(dirDistance.xyz);
//	pointDepth = point0ShadowMap.Sample(linearClampSampler, texPointCoords).x;
//	
//	depth = max(max(abs(dirDistance.x), abs(dirDistance.y)), abs(dirDistance.z));
//	if((depth / dirDistance.w) > pointDepth)
//		pointColor = float4(0, 0, 0, 0);
//	else
//		pointColor = ApplyPointLightPS(pointLight, directionToCamera, halfVec, dirDistance, 
//			normal, ambientMatColor, diffuseMatColor, specularMatColor);
//	return pointColor;
//}

float4 ApplySpotLight(SpotLightCBuffer inSpotLight, float3 directionToCamera, float3 fragmentPositon,
					   float3 normal, float4 fragmentColor, float specularMaterial)
{
	float4 color = float4(0, 0, 0, 0);
    if(inSpotLight.enabled)
	{
		float4 diffuse = fragmentColor * float4(inSpotLight.diffuseColor, 1);
		float4 ambient = fragmentColor * float4(inSpotLight.ambientColor, 1);
		float4 specular = specularMaterial * float4(inSpotLight.specularColor, 1);

		float3 toLight = inSpotLight.position.xyz - fragmentPositon;
		float toLightLength = length(toLight);
		toLight/=toLightLength;

		float nDotL = saturate(dot(toLight, normal));
		
		if(nDotL > 0.0)
		{
			float spotEffect = dot(inSpotLight.direction.xyz, -toLight);
			if( spotEffect > inSpotLight.cutoff)
			{
				spotEffect = pow(abs(spotEffect), abs(inSpotLight.exponent));
				float attenuation = CalculateAttenuation(inSpotLight.attenuation, toLightLength, 
					inSpotLight.range, spotEffect);
				// Specular 
				float specMod = CalculateSpecularAmount(toLight, normal, fragmentPositon, 
					inSpotLight.specularIntensity, inSpotLight.specularPower);

				color.xyz = attenuation * (nDotL * diffuse.xyz + ambient.xyz + specMod * specular.xyz);
				color.a = attenuation * specMod + diffuse.a;
			}
		}
	}

	return color;
}

#endif //FORWARD_LIGHTING_SUPPORT_HLSLI