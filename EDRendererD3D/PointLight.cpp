#include "precompiled.h"
#include "PointLight.h"
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
	PointLight::PointLight()
	{
		renderShape = nullptr;
	}

	PointLight::PointLight(const PointLight &rhs)
	{
		renderShape = nullptr;
		*this = rhs;
	}

	PointLight::~PointLight()
	{
		delete renderShape;
		renderShape = nullptr;
	}

    PointLight &PointLight::operator=(const PointLight &rhs)
	{
		cBufferData.pointLight.enabled =			rhs.cBufferData.pointLight.enabled;
		cBufferData.pointLight.diffuseColor =		rhs.cBufferData.pointLight.diffuseColor; 
		cBufferData.pointLight.ambientColor =		rhs.cBufferData.pointLight.ambientColor; 
		cBufferData.pointLight.specularColor =		rhs.cBufferData.pointLight.specularColor; 
		cBufferData.pointLight.specularPower =		rhs.cBufferData.pointLight.specularPower;
		cBufferData.pointLight.specularIntensity =	rhs.cBufferData.pointLight.specularIntensity;
		cBufferData.pointLight.attenuation =		rhs.cBufferData.pointLight.attenuation;
		cBufferData.pointLight.range =				rhs.cBufferData.pointLight.range;

		XMMATRIX mat = XMMatrixScaling( cBufferData.pointLight.range, cBufferData.pointLight.range, 
			cBufferData.pointLight.range);

		delete renderShape;
		
		renderShape = new RenderShapeLight;
		renderShape->Initialize(rhs.renderShape->GetRenderFormHandles(), 
			rhs.renderShape->GetWorldMatrixPtr());
		SetWorldMatrixPtr(&XMFLOAT4X4(	1, 0, 0, 0, 
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1));
		renderShape->SetLight(this);

		D3D11_BUFFER_DESC bd;
		rhs.cBuffer->GetDesc(&bd);
		cBuffer.Release();
		HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &cBuffer));

		return *this;
	}

	void PointLight::ApplyLight()
	{
		XMFLOAT4 pos(&GetWorldMatrixPtr()->_41);
		cBufferData.pointLight.position = pos;

		D3D11_MAPPED_SUBRESOURCE edit;
		Renderer::theContextPtr->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, &cBufferData, sizeof(cBufferData));
		Renderer::theContextPtr->Unmap(cBuffer, 0);

		Renderer::theContextPtr->VSSetConstantBuffers(cBufferData.POINT_LIGHT_REGISTER_SLOT, 
			1, &cBuffer.p);
		Renderer::theContextPtr->PSSetConstantBuffers(cBufferData.POINT_LIGHT_REGISTER_SLOT, 
			1, &cBuffer.p);

		Float3 toCamera = ViewPortManager::GetReference().GetActiveViewPosition() 
			- *(Float3 *)&pos.x;
		float toCamMagSquared = toCamera.dotProduct(toCamera);
		float radSquared = cBufferData.pointLight.range * cBufferData.pointLight.range;
		if(radSquared > toCamMagSquared)
			techniqueNameOverride = "RenderPointLightInside";
		else
			techniqueNameOverride = "RenderPointLightOutside";
		AddToContextSet();
	}

	void PointLight::Initialize(const char *fileName, const DirectX::XMFLOAT3 *_diffuseColor, 
		const DirectX::XMFLOAT3 *_ambientColor, const DirectX::XMFLOAT3 *_specularColor, 
		float _specularPower, float _specularIntensisty, const DirectX::XMFLOAT3 *_attenuation,
		float _radius, bool _isEnabled)
	{
		cBufferData.pointLight.enabled = _isEnabled;

		cBufferData.pointLight.diffuseColor = *_diffuseColor; 
		cBufferData.pointLight.ambientColor = *_ambientColor; 
		cBufferData.pointLight.specularColor = *_specularColor; 
		cBufferData.pointLight.specularPower = _specularPower;
		cBufferData.pointLight.specularIntensity = _specularIntensisty;

		cBufferData.pointLight.attenuation = *_attenuation;
		cBufferData.pointLight.range = _radius;

		LoadModel(fileName);

		cBuffer.Release();
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(cBufferData);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &cBuffer));
	}

	const XMFLOAT4X4 *PointLight::GetWorldMatrixPtr()
	{ 
		return renderShape->GetWorldMatrixPtr(); 
	}
	void PointLight::SetWorldMatrixPtr(XMFLOAT4X4 * mat)
	{
		renderShape->SetWorldMatrix(mat);
	}
	void PointLight::AddToContextSet()
	{ 
		renderShape->AddToContextSet(); 
	}

	void PointLight::LoadModel(const char *fileName)
	{
		delete renderShape;

		EDUtilities::ContentHandle<RenderForm> formHandle =
			EDUtilities::ContentManager::LoadXML<RenderForm>(fileName);
		renderShape = (RenderShapeLight *)formHandle.GetContent()->GetNewRenderShape();
		std::vector<EDUtilities::ContentHandle<RenderForm> > forms;
		forms.push_back(formHandle);
		renderShape->Initialize(forms);

		SetWorldMatrixPtr(&XMFLOAT4X4(	1, 0, 0, 0, 
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1));
		renderShape->SetLight(this);
	}
}