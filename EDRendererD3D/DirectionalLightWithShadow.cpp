#include "precompiled.h"
#include "DirectionalLightWithShadow.h"
#include "Renderer.h"
#include "RenderShapeLight.h"

#include "../EDMath/Float3.h"
#include "../EDMath/Float4x4.h"
using EDMath::Float3;
using EDMath::Float4x4;
using namespace DirectX;

namespace EDRendererD3D
{
	DirectionalLightWithShadow::DirectionalLightWithShadow(void)
	{
		for (unsigned int i = 0; i < NumCascades; ++i)
			viewProjection[i] = nullptr;
		renderShape = nullptr;
	}

	DirectionalLightWithShadow::~DirectionalLightWithShadow()
	{
		delete renderShape;
		renderShape = nullptr;
	}

	void DirectionalLightWithShadow::Initialize(const char *fileName, XMFLOAT4X4 *_viewProjection,
		const XMFLOAT3 *_diffuseColor, const XMFLOAT3 *_ambientColor,	
		const XMFLOAT3 *_specularColor, float _specularPower, float _specularIntensisty, 
		UINT width, UINT length, DXGI_FORMAT format, bool _enabled)
	{
		cBufferData.DirLight.diffuseColor = *_diffuseColor; 
		cBufferData.DirLight.ambientColor = *_ambientColor; 
		cBufferData.DirLight.specularColor = *_specularColor; 
		cBufferData.DirLight.enabled = _enabled;
		cBufferData.DirLight.specularPower = _specularPower;
		cBufferData.DirLight.specularIntensity = _specularIntensisty;

		// Shadow map for each cascade
		for (unsigned int i = 0; i < NumCascades; ++i)
		{
			viewProjection[i] = &_viewProjection[i];
			renderTarget[i].Create(width, length);
			std::string shadowmapName = "DirShadowMap";
			shadowmapName += i;
			renderTargetViews[i].Create(width, length, format, shadowmapName.c_str());
			renderTarget[i].AddTarget(&renderTargetViews[i]);
		}

		delete renderShape;

		formHandle =
			EDUtilities::ContentManager::LoadXML<RenderForm>(fileName);
		renderShape = (RenderShapeLight *)formHandle.GetContent()->GetNewRenderShape();

		std::vector<EDUtilities::ContentHandle<RenderForm> > forms;
		forms.push_back(formHandle);
		renderShape->Initialize(forms);

		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(DirectionalLightCBuffer);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HRESULT hr = Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &cBuffer);
		HR(hr);
	}

	DirectionalLightWithShadow &DirectionalLightWithShadow::operator=(
		const DirectionalLightWithShadow &rhs)
	{
		D3D11_BUFFER_DESC bd;
		rhs.cBuffer->GetDesc(&bd);
		cBuffer.Release();
		HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &cBuffer));

		cBufferData = rhs.cBufferData;

		formHandle = rhs.formHandle;
		renderShape = (RenderShapeLight *)formHandle.GetContent()->GetNewRenderShape();
		std::vector<EDUtilities::ContentHandle<RenderForm> > forms;
		forms.push_back(formHandle);
		renderShape->Initialize(forms);

		for (unsigned int i = 0; i < NumCascades; ++i)
		{
			viewProjection[i] = rhs.viewProjection[i];
			renderTarget[i] = rhs.renderTarget[i];
			renderTargetViews[i] = rhs.renderTargetViews[i];
		}
		return *this;
	}

	void DirectionalLightWithShadow::ApplyLight()
	{
		cBufferData.DirLight.direction = XMFLOAT3(&GetWorldMatrixPtr()->_31);
		for (unsigned int i = 0; i < NumCascades; ++i)
			cBufferData.DirLight.viewProjection[i] = *viewProjection[i];

		ApplyCBuffer();

		AddToContextSet();
	}

	void DirectionalLightWithShadow::ApplyCBuffer()
	{
		if(0 == cBuffer)
			return;

		D3D11_MAPPED_SUBRESOURCE edit;
		Renderer::theContextPtr->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, &cBufferData, sizeof(cBufferData));
		Renderer::theContextPtr->Unmap(cBuffer, 0);

		Renderer::theContextPtr->VSSetConstantBuffers(cBufferData.DIRECTIONAL_LIGHT_SHADOW_REGISTER_SLOT, 1, &cBuffer.p);
		Renderer::theContextPtr->PSSetConstantBuffers(cBufferData.DIRECTIONAL_LIGHT_SHADOW_REGISTER_SLOT, 1, &cBuffer.p);
	}

	void DirectionalLightWithShadow::ActivateShadowTarget(unsigned int index)
	{
		cBufferData.DirLight.currentIndex = index;
		ApplyLight();

		ID3D11ShaderResourceView *srv[NumCascades] = { nullptr, };
		// Clear shader resource views before they are used as render targets
		/*for (unsigned int i = 0; i < NumCascades; ++i)
		{
			Renderer::theContextPtr->PSSetShaderResources(REGISTER_SLOT + i, 1, &srv);
		}
*/
		Renderer::theContextPtr->PSSetShaderResources(REGISTER_SLOT, NumCascades, srv);

		renderTarget[index].ActivateTarget();
		FLOAT color[4] = { 1, 1, 1, 1 };
		renderTarget[index].ClearRenderTargetView(color);
		renderTarget[index].ClearDepthStencilView(D3D11_CLEAR_DEPTH);
	}

	void DirectionalLightWithShadow::ActivateShadowTexture()
	{
		ID3D11ShaderResourceView *srv[NumCascades];
		for (unsigned int i = 0; i < NumCascades; ++i)
		{
			srv[i] = renderTarget[i].GetShaderResourceView();
		}
		Renderer::theContextPtr->PSSetShaderResources(REGISTER_SLOT, NumCascades, srv);
	}

	void DirectionalLightWithShadow::RevertShadowTexture()
	{
		ID3D11ShaderResourceView *srv = 0;
		Renderer::theContextPtr->PSSetShaderResources(REGISTER_SLOT, 1, &srv);
	}

	const XMFLOAT4X4 *DirectionalLightWithShadow::GetWorldMatrixPtr()
	{ 
		return renderShape->GetWorldMatrixPtr(); 
	}
	void DirectionalLightWithShadow::SetWorldMatrixPtr(const XMFLOAT4X4 * mat)
	{
		renderShape->SetWorldMatrix(mat);
	}
	void DirectionalLightWithShadow::AddToContextSet()
	{ 
		renderShape->AddToContextSet(); 
	}
}