#pragma once

#include "SpotLight.h"
#include "RenderTarget.h"
#include "LightBuffers.h"

namespace EDRendererD3D
{
	/// Implementation of a spot light with shadow mapping data.
	class SpotLightWithShadow : public SpotLight
	{
	private:
		cbSpotLightShadowData cBufferShadowData;
		CComPtr<ID3D11Buffer> cBufferShadow;
		
		RenderTarget *renderTargetPtr;

		DirectX::XMFLOAT4X4 projection;

		static const int REGISTER_SLOT = 14;

		void Initialize(const char *fileName, const DirectX::XMFLOAT4 *_diffuseColor, 
			const DirectX::XMFLOAT3 *_ambientColor, const DirectX::XMFLOAT3 *_specularColor, 
			float _specularPower, float _specularIntensisty, float _cutoff, float _exponent, 
			const DirectX::XMFLOAT3 *_attenuation, float _radius, bool _isEnabled = true) {}
	public:

        SpotLightWithShadow();
        SpotLightWithShadow(const SpotLightWithShadow &rhs);

        virtual ~SpotLightWithShadow();
        SpotLightWithShadow &operator=(const SpotLightWithShadow &rhs);

   		/// Applies the light on a given effect.
		/// This method should be called each frame to set the light parameters.
		/// \param effectPtr - The effect this light should be applied to.
		virtual void ApplyLight();

		void Initialize(const char *fileName, const DirectX::XMFLOAT3 *_diffuseColor, 
			const DirectX::XMFLOAT3 *_ambientColor, const DirectX::XMFLOAT3 *_specularColor, 
			const DirectX::XMFLOAT3 *_attenuation, float _specularPower, float _specularIntensisty,
			float _cutoff, float _exponent, float _radius, RenderTarget *_renderTargetPtr, bool _isEnabled = true);

		//virtual std::string GetTechniqueNameOverride() { return techniqueNameOverride; }

        /// Sets the shadow target as the current render target 
        /// and applies the light.
        void ActivateShadowTarget();

		void ActivateShadowTexture();
        void RevertShadowTexture();

        RenderTarget &GetRenderTarget() { return *renderTargetPtr; }

	};
}