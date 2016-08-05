#pragma once

#include "RenderTarget.h"
#include "RenderContext.h"
#include "LightBuffers.h"

namespace EDRendererD3D
{
	class RenderShapeLight;
	class RenderForm;
	/// Implementation of a point light with shadow mapping data.
	/// The Frame controls the location of the light.
	class DirectionalLightWithShadow : public ILight
    {
    private:
		cbDirectionalLight cBufferData;


		static const int NumCascades = 3;
		RenderTarget renderTarget[NumCascades];
		RenderTargetSurface renderTargetViews[NumCascades];
		DirectX::XMFLOAT4X4 *viewProjection[NumCascades];

		RenderShapeLight *renderShape;
		EDUtilities::ContentHandle<RenderForm> formHandle;


		CComPtr<ID3D11Buffer> cBuffer;

		static const int REGISTER_SLOT = 13;
    public:
		DirectionalLightWithShadow(void);
		virtual ~DirectionalLightWithShadow(void);

		void Initialize(const char *fileName, DirectX::XMFLOAT4X4 *_viewProjection,
			const DirectX::XMFLOAT3 *_diffuseColor, const DirectX::XMFLOAT3 *_ambientColor,	
			const DirectX::XMFLOAT3 *_specularColor, float _specularPower, 
			float _specularIntensisty, UINT width, UINT length, 
			DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT, bool _enabled = true);

        DirectionalLightWithShadow &operator=(const DirectionalLightWithShadow &rhs);

   		/// Applies the light on a given effect.
		/// This method should be called each frame to set the light parameters.
		/// \param effectPtr - The effect this light should be applied to.
		virtual void ApplyLight();

		void ApplyCBuffer();

		virtual void SetEnabled(bool _isEnabled)
		{
			cBufferData.DirLight.enabled = _isEnabled;
		}

		virtual const DirectX::XMFLOAT4X4 *GetWorldMatrixPtr();
		virtual void SetWorldMatrixPtr(const DirectX::XMFLOAT4X4 * mat);
		virtual void AddToContextSet();

        /// Sets the shadow target as the current render target 
        /// and applies the light.
		void ActivateShadowTarget(unsigned int index);

        void ActivateShadowTexture();
        void RevertShadowTexture();

        RenderTarget &GetRenderTarget(unsigned int index) { return renderTarget[index]; }
		void CloneBufferData(DirectionalLightCBuffer &clone) { clone = cBufferData.DirLight; }

    };
}