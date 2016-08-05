#pragma once

#include "PointLight.h"
#include "CubeRenderTarget.h"
#include "LightBuffers.h"

namespace EDRendererD3D
{
	/// Implementation of a point light with shadow mapping data.
	class PointLightWithShadow : public PointLight
    {
    private:
        cbPointLightShadowData cBufferShadowData;
		CComPtr<ID3D11Buffer> cBufferShadow;

        CubeRenderTarget renderTarget;
		CubeRenderTargetView renderTargetView;
   		
		// Register slot for the this objects shadow map
		static const int REGISTER_SLOT = 15;

        void UpdateMatrices();

		// Disable the point light initialize method
		void Initialize(const char *fileName, const DirectX::XMFLOAT4 *_diffuseColor, 
			const DirectX::XMFLOAT3 *_ambientColor, const DirectX::XMFLOAT3 *_specularColor, 
			float _specularPower, float _specularIntensisty, const DirectX::XMFLOAT3 *_attenuation,
			float _radius, bool _isEnabled = true) {} 
    public:
        PointLightWithShadow();
        PointLightWithShadow(const PointLightWithShadow &rhs);
        virtual ~PointLightWithShadow();

        PointLightWithShadow &operator=(const PointLightWithShadow &rhs);

   		/// Applies the light on a given effect.
		/// This method should be called each frame to set the light parameters.
		/// \param effectPtr - The effect this light should be applied to.
		virtual void ApplyLight();

		void Initialize(const char *fileName, const DirectX::XMFLOAT3 *_diffuseColor, 
			const DirectX::XMFLOAT3 *_ambientColor, const DirectX::XMFLOAT3 *_specularColor, 
			const DirectX::XMFLOAT3 *_attenuation, float _specularPower, float _specularIntensisty,
			float _radius, UINT size, DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT, 
			bool _isEnabled = true);

        /// Sets the shadow target as the current render target and applies the light.
        void ActivateShadowTarget();
		void ActivateShadowTexture();
        void RevertShadowTexture();

		void ClearShadowMap();

        RenderTarget &GetRenderTarget() { return renderTarget; }

    };
}