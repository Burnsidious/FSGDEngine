#include "precompiled.h"
#include "ForwardLightManager.h"

#include "..//EDRendererD3D/Renderer.h"
using namespace EDRendererD3D;

ForwardLightManager::ForwardLightManager(void)
{
	currentPointLightIndex = currentSpotLightIndex = 0;
	directionalLightPtr = nullptr;
}


ForwardLightManager::~ForwardLightManager(void)
{
}

void ForwardLightManager::Initialize(void)
{
	forwardLightBuffer.Release();
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(forwardLightBufferData);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &forwardLightBuffer));
}

void ForwardLightManager::AddPointLight(PointLight *light)
{
	if(currentPointLightIndex < NUM_FORWARD_LIGHTS)
		pointLightPtrs[currentPointLightIndex++] = light;
}

void ForwardLightManager::AddSpotLight(SpotLight *light)
{
	if(currentSpotLightIndex < NUM_FORWARD_LIGHTS)
		spotLightPtrs[currentSpotLightIndex++] = light;
}

void ForwardLightManager::AddDirectionalLight(DirectionalLightWithShadow *light)
{
	directionalLightPtr = light;
}

void ForwardLightManager::ApplyForwardLights(void)
{
	// TODO: Add support for fewer than max lights applied
	unsigned int pointIndex = 0;
	// Get light values for active light
	for(; pointIndex < currentPointLightIndex; ++pointIndex)
	{
		pointLightPtrs[pointIndex]->CloneBufferData(forwardLightBufferData.forwardPointLights[pointIndex]);
	}
	// Disable inactive point lights
	for(; pointIndex < NUM_FORWARD_LIGHTS; ++pointIndex)
	{
		forwardLightBufferData.forwardPointLights[pointIndex].enabled = false;
	}

	unsigned int spotIndex = 0;
	for(; spotIndex < currentSpotLightIndex; ++spotIndex)
	{
		spotLightPtrs[spotIndex]->CloneBufferData(forwardLightBufferData.forwardSpotLights[spotIndex]);
	}
	// Disable inactive spot lights
	for(; spotIndex < NUM_FORWARD_LIGHTS; ++spotIndex)
	{
		forwardLightBufferData.forwardSpotLights[spotIndex].enabled = false;
	}

	if(directionalLightPtr)
		directionalLightPtr->CloneBufferData(forwardLightBufferData.forwardDirLight);

	// Reset light indices 
	currentPointLightIndex = currentSpotLightIndex = 0;

	// Write buffers to gpu
	D3D11_MAPPED_SUBRESOURCE edit;
	Renderer::theContextPtr->Map(forwardLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
	memcpy(edit.pData, &forwardLightBufferData, sizeof(forwardLightBufferData));
	Renderer::theContextPtr->Unmap(forwardLightBuffer, 0);

	Renderer::theContextPtr->VSSetConstantBuffers(forwardLightBufferData.FORWARD_LIGHTS_REGISTER_SLOT, 
		1, &forwardLightBuffer.p);
	Renderer::theContextPtr->PSSetConstantBuffers(forwardLightBufferData.FORWARD_LIGHTS_REGISTER_SLOT, 
		1, &forwardLightBuffer.p);
}
