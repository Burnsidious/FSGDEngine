#ifndef LIT_TEXTURED_PS_HLSL
#define LIT_TEXTURED_PS_HLSL

#include "VertexLayouts.hlsli"
#include "ForwardlightingSupport.hlsli"

#include "../EDRendererD3D/LightBuffers.h"
#include "../EDRendererD3D/ConstantBuffers.h"

ForwardFragOut main(ForwardRenderVertexOut input)
{
	float alpha;
	float4 normal, vTexColor;
	ForwardFragOut outColor;

	vTexColor = diffuseMap.Sample(anisoWrapSampler, input.texCoord);
	alpha = vTexColor.a;
	normal = normalize(input.normal);
	outColor.normal = normal;

    float3 directionToCamera = normalize(gCameraPos - input.worldPos.xyz);
	float4 dirColor = ApplyDirectionalLight(forwardDirLight, directionToCamera, 
						input.worldPos.xyz,	normal, vTexColor);

	float4 pointColor = float4(0, 0, 0, 0);
	
	for(unsigned int i = 0; i < NUM_FORWARD_LIGHTS; ++i)
	{
		pointColor += ApplyPointLight(forwardPointLights[i], directionToCamera, 
			input.worldPos.xyz,	normal.xyz, vTexColor, 1);
	}
	
	float4 spotColor = float4(0, 0, 0, 0);
	for(i = 0; i < NUM_FORWARD_LIGHTS; ++i)
	{
		spotColor += ApplySpotLight(forwardSpotLights[i], directionToCamera, input.worldPos.xyz,
		normal.xyz, vTexColor, 1);
	}

	outColor.diffuse.xyz = dirColor.xyz + pointColor.xyz + spotColor.xyz;
	outColor.diffuse.a = vTexColor.a;//min(pointColor.a, min(dirColor.a, spotColor.a));

	outColor.depth = input.screenPos.z/input.screenPos.w;

	return outColor;
}

#endif //LIT_TEXTURED_PS_HLSL