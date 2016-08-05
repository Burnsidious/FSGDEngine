#include "precompiled.h"
#include "PointLightWithShadow.h"
#include "RenderShapeLight.h"
#include "RenderContext.h"
#include "ViewPortManager.h"
#include "RenderContext.h"

#include "../EDMath/Float3.h"
#include "../EDMath/Float4x4.h"

using namespace DirectX;
using EDMath::Float3;
using EDMath::Float4x4;

namespace EDRendererD3D
{
	PointLightWithShadow::PointLightWithShadow()
	{
		//lightModelPtr = nullptr;
	}
    
	PointLightWithShadow::PointLightWithShadow(const PointLightWithShadow &rhs)
	{
		renderShape = nullptr;
		*this = rhs;
	}

	PointLightWithShadow::~PointLightWithShadow()
	{
		delete renderShape;
		renderShape = nullptr;
	}

    PointLightWithShadow &PointLightWithShadow::operator=(const PointLightWithShadow &rhs)
	{
		((PointLight *)this)->operator=(rhs);

		D3D11_BUFFER_DESC bd;
		rhs.cBufferShadow->GetDesc(&bd);
		cBufferShadow.Release();
		HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &cBufferShadow));

		renderTarget = rhs.renderTarget;
		renderTargetView = rhs.renderTargetView;

		return *this;
	}

	void PointLightWithShadow::ApplyLight()
	{
		PointLight::ApplyLight();
		UpdateMatrices();
		D3D11_MAPPED_SUBRESOURCE edit;
		Renderer::theContextPtr->Map(cBufferShadow, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, &cBufferShadowData, sizeof(cBufferShadowData));
		Renderer::theContextPtr->Unmap(cBufferShadow, 0);

		Renderer::theContextPtr->VSSetConstantBuffers(cBufferShadowData.POINT_LIGHT_SHADOW_REGISTER_SLOT, 
			1, &cBufferShadow.p);
		Renderer::theContextPtr->PSSetConstantBuffers(cBufferShadowData.POINT_LIGHT_SHADOW_REGISTER_SLOT, 
			1, &cBufferShadow.p);
		Renderer::theContextPtr->GSSetConstantBuffers(cBufferShadowData.POINT_LIGHT_SHADOW_REGISTER_SLOT, 
			1, &cBufferShadow.p);
	}

	void PointLightWithShadow::Initialize(const char *fileName, const DirectX::XMFLOAT3 *_diffuseColor, 
		const DirectX::XMFLOAT3 *_ambientColor, const DirectX::XMFLOAT3 *_specularColor, 
		const DirectX::XMFLOAT3 *_attenuation, float _specularPower, float _specularIntensisty,
		float _radius, UINT size, DXGI_FORMAT format, bool _isEnabled)
	{
		PointLight::Initialize(fileName, _diffuseColor, _ambientColor, _specularColor, _specularPower,
			_specularIntensisty, _attenuation, _radius, _isEnabled);

		cBufferShadow.Release();
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(cBufferShadowData);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &cBufferShadow));

		renderTarget.Create(size);
		renderTargetView.Create(size, format, "Point Light Shadow Map");
		renderTarget.AddTarget(&renderTargetView);
	}

	void PointLightWithShadow::ActivateShadowTarget()
	{
		ApplyLight();

		renderTarget.ActivateTarget();

		renderTarget.ClearDepthStencilView(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	}

	void PointLightWithShadow::ClearShadowMap()
	{
		FLOAT color[4] = { 1, 1, 1, 1 };
		for(unsigned int i = 0; i < 6; ++i)
		{
			renderTarget.ClearRenderTargetView(color, i);
		}
	}

	void PointLightWithShadow::UpdateMatrices()
	{
		XMMATRIX viewMat;
		XMFLOAT3 look, up, eye;
		eye.x = GetWorldMatrixPtr()->_41;
		eye.y = GetWorldMatrixPtr()->_42;
		eye.z = GetWorldMatrixPtr()->_43;
		float nearClip = 0.1f;
		float farClip = GetRange();

		cBufferShadowData.pointLightShadowData.shadowBias = 0.001f;

		// Create log and linear conversion values
		cBufferShadowData.pointLightShadowData.planeX = -farClip / (farClip - nearClip);
		cBufferShadowData.pointLightShadowData.planeY = 
			-farClip * nearClip / (farClip - nearClip);

		// Build projection
		XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PI / 2.0f, 1.0f, nearClip, farClip);


		Float4x4 worldMat;

		// Right
		worldMat.makeIdentity();
		worldMat.XAxis = Float3(0, 0, -1);
		worldMat.YAxis = Float3(0, 1, 0);
		worldMat.ZAxis = Float3(1, 0, 0);
		worldMat.WAxis = *(Float3 *)&eye;
		Float4x4 invertMat = worldMat;
		invertMat.transpose3x3();

		invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
		invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
		invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
		viewMat = XMLoadFloat4x4((XMFLOAT4X4 *)&invertMat);

		XMStoreFloat4x4(&cBufferShadowData.pointLightShadowData.viewProjection[0], 
			XMMatrixMultiply(viewMat, projection));

		// Left
		worldMat.makeIdentity();
		worldMat.XAxis = Float3(0, 0, 1);
		worldMat.YAxis = Float3(0, 1, 0);
		worldMat.ZAxis = Float3(-1, 0, 0);
		worldMat.WAxis = *(Float3 *)&eye;
		invertMat = worldMat;
		invertMat.transpose3x3();

		invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
		invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
		invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
		viewMat = XMLoadFloat4x4((XMFLOAT4X4 *)&invertMat);


		XMStoreFloat4x4(&cBufferShadowData.pointLightShadowData.viewProjection[1], 
			XMMatrixMultiply(viewMat, projection));

		// Up
		worldMat.makeIdentity();
		worldMat.XAxis = Float3(1, 0, 0);
		worldMat.YAxis = Float3(0, 0, -1);
		worldMat.ZAxis = Float3(0, 1, 0);
		worldMat.WAxis = *(Float3 *)&eye;
		invertMat = worldMat;
		invertMat.transpose3x3();

		invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
		invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
		invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
		viewMat = XMLoadFloat4x4((XMFLOAT4X4 *)&invertMat);


		XMStoreFloat4x4(&cBufferShadowData.pointLightShadowData.viewProjection[2], 
			XMMatrixMultiply(viewMat, projection));

		// Down
		worldMat.makeIdentity();
		worldMat.XAxis = Float3(1, 0, 0);
		worldMat.YAxis = Float3(0, 0, 1);
		worldMat.ZAxis = Float3(0, -1, 0);
		worldMat.WAxis = *(Float3 *)&eye;
		invertMat = worldMat;
		invertMat.transpose3x3();

		invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
		invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
		invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
		viewMat = XMLoadFloat4x4((XMFLOAT4X4 *)&invertMat);

		XMStoreFloat4x4(&cBufferShadowData.pointLightShadowData.viewProjection[3], 
			XMMatrixMultiply(viewMat, projection));

		// Forward
		worldMat.makeIdentity();
		worldMat.WAxis = *(Float3 *)&eye;
		invertMat = worldMat;
		invertMat.transpose3x3();

		invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
		invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
		invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
		viewMat = XMLoadFloat4x4((XMFLOAT4X4 *)&invertMat);


		XMStoreFloat4x4(&cBufferShadowData.pointLightShadowData.viewProjection[4], 
			XMMatrixMultiply(viewMat, projection));

		// Backward
		worldMat.makeIdentity();
		worldMat.XAxis = Float3(-1, 0, 0);
		worldMat.YAxis = Float3(0, 1, 0);
		worldMat.ZAxis = Float3(0, 0, -1);
		worldMat.WAxis = *(Float3 *)&eye;
		invertMat = worldMat;
		invertMat.transpose3x3();

		invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
		invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
		invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
		viewMat = XMLoadFloat4x4((XMFLOAT4X4 *)&invertMat);


		XMStoreFloat4x4(&cBufferShadowData.pointLightShadowData.viewProjection[5], 
			XMMatrixMultiply(viewMat, projection));

	}

	void PointLightWithShadow::ActivateShadowTexture()
	{
		ID3D11ShaderResourceView *srv = renderTarget.GetShaderResourceView();
		Renderer::theContextPtr->PSSetShaderResources(REGISTER_SLOT, 1, &srv);
	}

	void PointLightWithShadow::RevertShadowTexture()
	{
		ID3D11ShaderResourceView *srv = 0;
		Renderer::theContextPtr->PSSetShaderResources(REGISTER_SLOT, 1, &srv);
	}
}