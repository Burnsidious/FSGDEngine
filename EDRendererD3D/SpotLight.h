#pragma once

#include "Light.h"
#include "RenderTarget.h"
#include "LightBuffers.h"

namespace EDRendererD3D
{
	class RenderShapeLight;

	/// Implementation of a spot light
	class SpotLight : public ILight
	{
	protected:
		cbSpotLight cBufferData;

		RenderShapeLight *renderShape;
		CComPtr<ID3D11Buffer> cBuffer;

		std::string techniqueNameOverride;

        void LoadModel(const char *fileName);

		void SetEnabled(bool _isEnabled)
		{
			cBufferData.spotLight.enabled = _isEnabled;
		}
	public:
        SpotLight();
        SpotLight(const SpotLight &rhs);
        virtual ~SpotLight();
        SpotLight &operator=(const SpotLight &rhs);

   		/// Applies the light on a given effect.
		/// This method should be called each frame to set the light parameters.
		/// \param effectPtr - The effect this light should be applied to.
		virtual void ApplyLight();

		void Initialize(const char *fileName, const DirectX::XMFLOAT3 *_diffuseColor, 
			const DirectX::XMFLOAT3 *_ambientColor, const DirectX::XMFLOAT3 *_specularColor, 
			float _specularPower, float _specularIntensisty, float _cutoff, float _exponent, 
			const DirectX::XMFLOAT3 *_attenuation, float _radius, bool _isEnabled = true);

		void SetWorldMatrixPtr(DirectX::XMFLOAT4X4 * mat);

		const float3& GetDiffuseColor(void)const
		{
			return cBufferData.spotLight.diffuseColor;
		}
        const float3& GetAmbientColor(void)const
        {
			return cBufferData.spotLight.ambientColor;
		}
		const float3& GetSpecularColor(void)const
        {
			return cBufferData.spotLight.specularColor;
		}
		int GetEnabled(void)const
		{
			return cBufferData.spotLight.enabled;
		}
		float GetSpecularPower(void)const
		{
			return cBufferData.spotLight.specularPower;
		}
        float GetSpecularIntensity(void)const
		{
			return cBufferData.spotLight.specularIntensity;
		}

		float GetExponent(void)const
		{ 
			return cBufferData.spotLight.exponent; 
		}


		DirectX::XMVECTOR GetAttenuation(void)
		{ 
			return DirectX::XMLoadFloat3(&cBufferData.spotLight.attenuation); 
		}
		const float3& GetAttenuationf(void)const
		{ 
			return cBufferData.spotLight.attenuation; 
		}

		float GetCutOff(void)
		{
			return cBufferData.spotLight.cutoff; 
		}
		float GetRange(void)const
		{
			return cBufferData.spotLight.range; 
		}

		std::string GetTechniqueNameOverride() 
		{
			return techniqueNameOverride; 
		}

		const DirectX::XMFLOAT4X4 *GetWorldMatrixPtr();
		void AddToContextSet();

		void CloneBufferData(SpotLightCBuffer &clone) { clone = cBufferData.spotLight; }
	};
}