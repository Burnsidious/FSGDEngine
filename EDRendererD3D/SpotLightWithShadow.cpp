#include "precompiled.h"
#include "SpotLightWithShadow.h"
#include "RenderShapeLight.h"
#include "ViewPortManager.h"

#include "../EDMath/Float3.h"
#include "../EDMath/Float4x4.h"
using EDMath::Float3;
using EDMath::Float4x4;

using namespace DirectX;

namespace EDRendererD3D
{
	SpotLightWithShadow::SpotLightWithShadow()
	{
		renderTargetPtr = nullptr;
	}

	SpotLightWithShadow::SpotLightWithShadow(const SpotLightWithShadow &rhs)
	{
		renderShape = nullptr;
		*this = rhs;
	}

	SpotLightWithShadow::~SpotLightWithShadow()
	{
		delete renderShape;
		renderShape = nullptr;
	}
	
    SpotLightWithShadow &SpotLightWithShadow::operator=(const SpotLightWithShadow &rhs)
	{
		SpotLight::operator=(rhs);

		float fov = acos(cBufferData.spotLight.cutoff) * 2;
		projection = *(XMFLOAT4X4*)&XMMatrixPerspectiveFovLH(fov, 1, 0.1f, 
			cBufferData.spotLight.range);

		renderTargetPtr = rhs.renderTargetPtr;

		D3D11_BUFFER_DESC bd;
		rhs.cBufferShadow->GetDesc(&bd);
		cBufferShadow.Release();
		HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &cBufferShadow));

		return *this;
	}
	
	void SpotLightWithShadow::Initialize(const char *fileName, const DirectX::XMFLOAT3 *_diffuseColor, 
			const DirectX::XMFLOAT3 *_ambientColor, const DirectX::XMFLOAT3 *_specularColor, 
			const DirectX::XMFLOAT3 *_attenuation, float _specularPower, float _specularIntensisty,
			float _cutoff, float _exponent, float _radius, RenderTarget *_renderTargetPtr, bool _isEnabled)
	{
		SpotLight::Initialize(fileName, _diffuseColor, _ambientColor, _specularColor, _specularPower,
			_specularIntensisty, _cutoff, _exponent, _attenuation, _radius, _isEnabled);

		cBufferShadow.Release();
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(cBufferShadowData);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &cBufferShadow));

		renderTargetPtr = _renderTargetPtr;

		float fov = acos(cBufferData.spotLight.cutoff) * 2;

		projection = *(XMFLOAT4X4*)&XMMatrixPerspectiveFovLH(fov, 1, 0.1f, 
			cBufferData.spotLight.range);
	}

	void SpotLightWithShadow::ApplyLight()
	{
		SpotLight::ApplyLight();

		// Build viewProjection
		Float4x4 invertMat(*(Float4x4*)GetWorldMatrixPtr());
		// Remove scale before we inverse
		Float3 scale = invertMat.getScale();
		invertMat.scale(1.0f/scale.x, 1.0f/scale.y, 1.0f/scale.z);

		Float3 xAxis = invertMat.XAxis;
		Float3 yAxis = invertMat.YAxis;
		Float3 zAxis = invertMat.ZAxis;
		Float3 wAxis = invertMat.WAxis;

		invertMat.transpose3x3();

		invertMat.WAxis.x = -DotProduct(wAxis, xAxis);
	    invertMat.WAxis.y = -DotProduct(wAxis, yAxis);
	    invertMat.WAxis.z = -DotProduct(wAxis, zAxis);

		XMMATRIX tempInvertView = XMLoadFloat4x4((XMFLOAT4X4 *)&invertMat);

		XMMATRIX tempMult = XMMatrixMultiply(tempInvertView, XMLoadFloat4x4(&projection));
		XMStoreFloat4x4(&cBufferShadowData.spotLightShadowData.viewProjection, 
			tempMult);

		D3D11_MAPPED_SUBRESOURCE edit;
		Renderer::theContextPtr->Map(cBufferShadow, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, &cBufferShadowData, sizeof(cBufferShadowData));
		Renderer::theContextPtr->Unmap(cBufferShadow, 0);

		Renderer::theContextPtr->VSSetConstantBuffers(cBufferShadowData.SPOT_LIGHT_SHADOW_REGISTER_SLOT, 
			1, &cBufferShadow.p);
		Renderer::theContextPtr->PSSetConstantBuffers(cBufferShadowData.SPOT_LIGHT_SHADOW_REGISTER_SLOT, 
			1, &cBufferShadow.p);
	}

	void SpotLightWithShadow::ActivateShadowTarget()
	{
        ApplyLight();

        renderTargetPtr->ActivateTarget();
		FLOAT color[4] = { 1, 1, 1, 1 };
		renderTargetPtr->ClearRenderTargetView(color);
		renderTargetPtr->ClearDepthStencilView(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	}

	void SpotLightWithShadow::ActivateShadowTexture()
	{
		ID3D11ShaderResourceView *srv = renderTargetPtr->GetShaderResourceView();
		Renderer::theContextPtr->PSSetShaderResources(REGISTER_SLOT, 1, &srv);
	}

	void SpotLightWithShadow::RevertShadowTexture()
	{
		ID3D11ShaderResourceView *srv = 0;
		Renderer::theContextPtr->PSSetShaderResources(REGISTER_SLOT, 1, &srv);
	}
}