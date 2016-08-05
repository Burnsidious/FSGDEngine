#pragma once

#include "..//EDRendererD3D/LightBuffers.h"
#include "..//EDRendererD3D/PointLight.h"
#include "..//EDRendererD3D/SpotLight.h"
#include "..//EDRendererD3D/DirectionalLightWithShadow.h"

class ForwardLightManager
{
public:
	ForwardLightManager(void);
	~ForwardLightManager(void);

	void Initialize(void);
	void AddPointLight(EDRendererD3D::PointLight *light);
	void AddSpotLight(EDRendererD3D::SpotLight *light);
	void AddDirectionalLight(EDRendererD3D::DirectionalLightWithShadow *light);

	void ApplyForwardLights(void);
private:
	unsigned int currentPointLightIndex;
	EDRendererD3D::PointLight *pointLightPtrs[NUM_FORWARD_LIGHTS];
	unsigned int currentSpotLightIndex;
	EDRendererD3D::SpotLight *spotLightPtrs[NUM_FORWARD_LIGHTS];
	EDRendererD3D::DirectionalLightWithShadow *directionalLightPtr;

	cbForwardLights forwardLightBufferData;
	CComPtr<ID3D11Buffer> forwardLightBuffer;

};

