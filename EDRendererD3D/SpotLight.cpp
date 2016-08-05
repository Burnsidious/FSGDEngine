#include "precompiled.h"
#include "SpotLight.h"
#include "RenderShapeLight.h"
#include "ViewPortManager.h"

#include "../EDMath/Float3.h"
#include "../EDMath/Float4x4.h"
using EDMath::Float3;
using EDMath::Float4x4;

using namespace DirectX;

namespace EDRendererD3D
{
	SpotLight::SpotLight()
	{
		renderShape = nullptr;
	}
	SpotLight::SpotLight(const SpotLight &rhs)
	{
		renderShape = nullptr;
		*this = rhs;
	}
	SpotLight::~SpotLight()
	{
		delete renderShape;
		renderShape = nullptr;
	}
	
    SpotLight &SpotLight::operator=(const SpotLight &rhs)
	{
		cBufferData.spotLight.enabled =				rhs.cBufferData.spotLight.enabled;
		cBufferData.spotLight.diffuseColor =		rhs.cBufferData.spotLight.diffuseColor; 
		cBufferData.spotLight.ambientColor =		rhs.cBufferData.spotLight.ambientColor; 
		cBufferData.spotLight.specularColor =		rhs.cBufferData.spotLight.specularColor; 
		cBufferData.spotLight.specularPower =		rhs.cBufferData.spotLight.specularPower;
		cBufferData.spotLight.specularIntensity =	rhs.cBufferData.spotLight.specularIntensity;
		cBufferData.spotLight.attenuation =			rhs.cBufferData.spotLight.attenuation;
		cBufferData.spotLight.range =				rhs.cBufferData.spotLight.range;
		cBufferData.spotLight.cutoff =				rhs.cBufferData.spotLight.cutoff;
		cBufferData.spotLight.exponent =			rhs.cBufferData.spotLight.exponent;

		XMMATRIX mat = XMMatrixScaling( cBufferData.spotLight.range, cBufferData.spotLight.range, 
			cBufferData.spotLight.range);


		delete renderShape;
		
		this->renderShape = new RenderShapeLight;
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
	void SpotLight::ApplyLight()
	{
		// Set spot light position and direction
		Float3 dir3(GetWorldMatrixPtr()->_31, GetWorldMatrixPtr()->_32, GetWorldMatrixPtr()->_33);
		dir3.normalize();

		XMFLOAT3 pos(&GetWorldMatrixPtr()->_41);

		cBufferData.spotLight.direction = XMFLOAT3(dir3.x, dir3.y, dir3.z);
		cBufferData.spotLight.position = pos;

		D3D11_MAPPED_SUBRESOURCE edit;
		Renderer::theContextPtr->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, &cBufferData, sizeof(cBufferData));
		Renderer::theContextPtr->Unmap(cBuffer, 0);

		Renderer::theContextPtr->VSSetConstantBuffers(cBufferData.SPOT_LIGHT_REGISTER_SLOT, 1, &cBuffer.p);
		Renderer::theContextPtr->PSSetConstantBuffers(cBufferData.SPOT_LIGHT_REGISTER_SLOT, 1, &cBuffer.p);


		// Check if the camera is inside the lit area
		Float3 toCamera = ViewPortManager::GetReference().GetActiveViewPosition() 
			- *(Float3 *)&pos.x;
		toCamera.normalize();

		if(toCamera.dotProduct(dir3) > cBufferData.spotLight.cutoff)
			techniqueNameOverride = "RenderSpotLightInside";
		else
			techniqueNameOverride = "RenderSpotLightOutside";
		AddToContextSet();
	}

	void SpotLight::Initialize(const char *fileName, const DirectX::XMFLOAT3 *_diffuseColor, 
		const DirectX::XMFLOAT3 *_ambientColor, const DirectX::XMFLOAT3 *_specularColor, 
		float _specularPower, float _specularIntensisty, float _cutoff, float _exponent, 
		const DirectX::XMFLOAT3 *_attenuation, float _radius, bool _isEnabled)
	{
		cBufferData.spotLight.enabled = _isEnabled;
		cBufferData.spotLight.diffuseColor = *_diffuseColor; 
		cBufferData.spotLight.ambientColor = *_ambientColor; 
		cBufferData.spotLight.specularColor = *_specularColor; 
		cBufferData.spotLight.specularPower = _specularPower;
		cBufferData.spotLight.specularIntensity = _specularIntensisty;
		cBufferData.spotLight.exponent = _exponent;
		cBufferData.spotLight.cutoff = _cutoff;

		LoadModel(fileName);

		cBufferData.spotLight.attenuation = *_attenuation;
		float adjacent = cBufferData.spotLight.range = _radius;

		float radAngle = acos(cBufferData.spotLight.cutoff);
		float tanr = tan(radAngle);
		float opposite = tanr * adjacent;

		XMMATRIX mat = XMMatrixScaling( opposite, opposite, adjacent);
		renderShape->SetWorldMatrix(((XMFLOAT4X4 *)&mat));

		cBuffer.Release();
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(cBufferData);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &cBuffer));
	}

	const XMFLOAT4X4 *SpotLight::GetWorldMatrixPtr()
	{ 
		return renderShape->GetWorldMatrixPtr(); 
	}
	void SpotLight::SetWorldMatrixPtr(XMFLOAT4X4 * mat)
	{
		renderShape->SetWorldMatrix(mat);
	}
	void SpotLight::AddToContextSet()
	{ 
		renderShape->AddToContextSet(); 
	}

	void SpotLight::LoadModel(const char *fileName)
    {
		delete renderShape;

		EDUtilities::ContentHandle<RenderForm> formHandle =
			EDUtilities::ContentManager::LoadXML<RenderForm>(fileName);
		renderShape = (RenderShapeLight *)formHandle.GetContent()->GetNewRenderShape();
		std::vector<EDUtilities::ContentHandle<RenderForm> > forms;
		forms.push_back(formHandle);
		renderShape->Initialize(forms);

		renderShape->SetLight(this);
    }
}