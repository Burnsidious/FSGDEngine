#ifndef LIGHT_BUFFERS_H
#define LIGHT_BUFFERS_H

#include "SharedDefines.h"

struct _regAlign DirectionalLightCBuffer
{
	/// A projection matrix defined by this lights properties, used for shadow mapping.
	float4x4	viewProjection[3];
	/// The direction the light shines
	float3		direction;
	/// If false this light will not be used
	int			enabled;
	/// The color of the ambient component of the light.
	float3		ambientColor;
	/// Controls how big the specular reflection is. A smaller value creates a large reflection.
	float		specularPower;
	/// The color of the specular component of the light.
	float3		specularColor;
	/// Controls the brightness of the specualr reflection.
	float		specularIntensity;
	/// The color of the diffuse component of the light.
	float3		diffuseColor;

	int currentIndex;
};

struct _regAlign PointLightShadowDataCBuffer
{
	float4x4 viewProjection[6];
	float planeX;
	float planeY;
	float shadowBias;
};

struct _regAlign PointLightCBuffer
{
	/// The position the light shines from.
	float4 position;
	/// The color of the diffuse component of the light.
	float3 diffuseColor;
	/// If false this light will not be used
	int enabled;
	/// The color of the ambient component of the light.
	float3 ambientColor;
	/// The maximum distance this light should effect, adjusts the resulting attenuation to match
	float range;
	/// The color of the specular component of the light.
	float3 specularColor;
	/// Controls how big the specular reflection is. A smaller value creates a large reflection.
	float specularPower;
	///(x = constant attenuation, y = linear attenuation, z = quadratic attenuation)
	float3 attenuation;
	/// Controls the brightness of the specualr reflection.
	float specularIntensity;
};

struct _regAlign SpotLightShadowDataCBuffer
{
	/// A projection matrix defined by this lights properties, used for shadow mapping.
	float4x4 viewProjection;
};

struct _regAlign SpotLightCBuffer
{
	/// The position the light shines from.
	float3 position;
	/// Controls how the light attenuates as ig gets further from the direction.
	/// A larger value creates a faster fall off of brightness.
	float exponent;
	/// The forward direction the light shines at.
	float3 direction;
	/// If false this light will not be used
	int enabled;
	/// The color of the diffuse component of the light.
	float3 diffuseColor;
	/// Controls how wide the angle of the spot is.
	/// This is the cosine of the angle between the direction and the furthest point to be lit.
	float cutoff;
	/// The color of the ambient component of the light.
	float3 ambientColor;
	/// Controls how big the specular reflection is. A smaller value creates a large reflection.
	float specularPower;
	/// The color of the specular component of the light.
	float3 specularColor;
	/// Controls the brightness of the specualr reflection.
	float specularIntensity;
	///(x = constant attenuation, y = linear attenuation, z = quadratic attenuation)
	float3 attenuation;
	/// The maximum distance this light should effect, adjusts the resulting attenuation to match
	float range;
};

CONSTANT_BUFFER_BEGIN(cbDirectionalLight, b5)
	DirectionalLightCBuffer DirLight;
	/// REGISTER_SLOT must match the register used shader side
	const static int DIRECTIONAL_LIGHT_SHADOW_REGISTER_SLOT = 5;
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbPointLightShadowData, b6)
	PointLightShadowDataCBuffer pointLightShadowData;
	const static int POINT_LIGHT_SHADOW_REGISTER_SLOT = 6;
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbPointLight, b7)
	PointLightCBuffer pointLight;
	const static int POINT_LIGHT_REGISTER_SLOT = 7;
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbSpotLightShadowData, b8)
	SpotLightShadowDataCBuffer spotLightShadowData;
	const static int SPOT_LIGHT_SHADOW_REGISTER_SLOT = 8;
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbSpotLight, b9)
	SpotLightCBuffer spotLight;
	const static int SPOT_LIGHT_REGISTER_SLOT = 9;
CONSTANT_BUFFER_END

	static const unsigned int NUM_FORWARD_LIGHTS = 8;
CONSTANT_BUFFER_BEGIN(cbForwardLights, b5)
	PointLightCBuffer forwardPointLights[NUM_FORWARD_LIGHTS];
	//PointLightShadowDataCBuffer forwardPointLightShadowDatas[NUM_FORWARD_LIGHTS];
	SpotLightCBuffer forwardSpotLights[NUM_FORWARD_LIGHTS];
	//SpotLightShadowDataCBuffer forwardSpotLightShadowDatas[NUM_FORWARD_LIGHTS];
	DirectionalLightCBuffer forwardDirLight;
	const static int FORWARD_LIGHTS_REGISTER_SLOT = 5;
CONSTANT_BUFFER_END


#endif //LIGHT_BUFFERS_H