#pragma once

#include "Light.h"
#include "CubeRenderTarget.h"
#include "LightBuffers.h"

namespace EDRendererD3D
{
	class RenderShapeLight;
	/// Implementation of a point light
	class PointLight : public ILight
    {
    protected:
        cbPointLight cBufferData;

		RenderShapeLight *renderShape;
		CComPtr<ID3D11Buffer> cBuffer;

		std::string techniqueNameOverride;
				
		void SetEnabled(bool _isEnabled)
		{
			cBufferData.pointLight.enabled = _isEnabled;
		}

        void LoadModel(const char *fileName);
    public:
        PointLight();
        PointLight(const PointLight &rhs);
        virtual ~PointLight();
        PointLight &operator=(const PointLight &rhs);

   		/// Applies the light on a given effect.
		/// This method should be called each frame to set the light parameters.
		/// \param effectPtr - The effect this light should be applied to.
		virtual void ApplyLight();

		void Initialize(const char *fileName, const DirectX::XMFLOAT3 *_diffuseColor, 
			const DirectX::XMFLOAT3 *_ambientColor, const DirectX::XMFLOAT3 *_specularColor, 
			float _specularPower, float _specularIntensisty, const DirectX::XMFLOAT3 *_attenuation,
			float _radius, bool _isEnabled = true);

		void SetWorldMatrixPtr(DirectX::XMFLOAT4X4 * mat);

        const float3& GetDiffuseColor(void)const
		{
			return cBufferData.pointLight.diffuseColor;
		}
        const float3& GetAmbientColor(void)const
        {
			return cBufferData.pointLight.ambientColor;
		}
		const float3& GetSpecularColor(void)const
        {
			return cBufferData.pointLight.specularColor;
		}
		int GetEnabled(void)const
		{
			return cBufferData.pointLight.enabled;
		}
		float GetSpecularPower(void)const
		{
			return cBufferData.pointLight.specularPower;
		}
        float GetSpecularIntensity(void)const
		{
			return cBufferData.pointLight.specularIntensity;
		}		
		float GetRange() { return cBufferData.pointLight.range; }
		const float3& GetAttenuationf(void)const{ return cBufferData.pointLight.attenuation; }

		const DirectX::XMFLOAT4X4 *GetWorldMatrixPtr();
		void AddToContextSet();

		inline DirectX::XMVECTOR GetAttenuation(void) 
		{ 
			return XMLoadFloat3(&cBufferData.pointLight.attenuation); 
		}

		std::string GetTechniqueNameOverride() { return techniqueNameOverride; }

		void CloneBufferData(PointLightCBuffer &clone) { clone = cBufferData.pointLight; }
    };
}