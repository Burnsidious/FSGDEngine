#pragma once


#include <DirectXMath.h>
#include <DirectXColors.h>

#include "../DirectXTK/Inc/SpriteBatch.h"

//using DirectX::FXMVECTOR;
using namespace DirectX;//::Colors;
using namespace DirectX::Colors;

namespace EDRendererD3D
{
	class RenderSprite
	{
		friend class RenderSpriteManager;
	public:
		RenderSprite();
		~RenderSprite(void);

		void Initialize(CComPtr<ID3D11ShaderResourceView> _textureSRV, const RECT &_destinationRect,
			RECT *_sourceRect = nullptr, XMFLOAT4 _color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 
			float _rotation = 0, DirectX::XMFLOAT2 const &_origin = Float2Zero, 
			DirectX::SpriteEffects _effects = DirectX::SpriteEffects_None, float _layerDepth = 0);

		inline void SetTexture(CComPtr<ID3D11ShaderResourceView> _textureSRV) { textureSRV = _textureSRV; }
		inline void SetTexture(ID3D11ShaderResourceView *_textureSRV) { textureSRV = _textureSRV; }
		inline const CComPtr<ID3D11ShaderResourceView> GetTexture(void) const { return textureSRV; }
		inline CComPtr<ID3D11ShaderResourceView> GetTexture(void) { return textureSRV; }

		inline void SetDestinationRect(RECT &_destinationRect) { destinationRect = _destinationRect; }
		inline void SetDestinationRect(LONG left, LONG right, LONG top, LONG bottom) 
		{ 
			destinationRect.left = left; 
			destinationRect.right = right; 
			destinationRect.top = top; 
			destinationRect.bottom = bottom; 
		}
		inline const RECT &GetDestinationRect(void) const { return destinationRect; }
		inline RECT &GetDestinationRect(void) { return destinationRect; }

		inline void SetSourceRect(const RECT *_soureRect) 
		{
			delete sourceRect;
			sourceRect = new RECT;
			*sourceRect = *_soureRect;
		}
		inline RECT *GetSourceRect(void) { return sourceRect; }
		inline const RECT *GetSourceRect(void) const { return sourceRect; }

		inline void SetShouldRender(bool _shouldRender) { shouldRender = _shouldRender; }
		inline bool GetShouldRender(void) { return shouldRender; }
	private:
		static const DirectX::XMFLOAT2 Float2Zero;

		CComPtr<ID3D11ShaderResourceView> textureSRV;
		RECT destinationRect;
		RECT *sourceRect;
		DirectX::XMFLOAT4 color;
		float rotation;
		DirectX::XMFLOAT2 origin;
		DirectX::SpriteEffects effects;
		float layerDepth;

		bool shouldRender;
	};
}
