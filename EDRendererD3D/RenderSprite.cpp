#include "precompiled.h"
#include "RenderSprite.h"
#include "RenderSpriteManager.h"

using namespace DirectX;

namespace EDRendererD3D
{
	const XMFLOAT2 RenderSprite::Float2Zero(0, 0);

	RenderSprite::RenderSprite()
	{
		RenderSpriteManager::GetReference().AddSprite(this);
		shouldRender = false;
	}

	RenderSprite::~RenderSprite(void)
	{
		delete sourceRect;
	}

	void RenderSprite::Initialize(CComPtr<ID3D11ShaderResourceView> _textureSRV, 
		const RECT &_destinationRect, RECT *_sourceRect, XMFLOAT4 _color, 
		float _rotation, DirectX::XMFLOAT2 const &_origin, 
		DirectX::SpriteEffects _effects, float _layerDepth)
	{
		textureSRV = _textureSRV;
		destinationRect = _destinationRect;
		sourceRect = _sourceRect;
		color = _color;
		rotation = _rotation;
		origin = _origin;
		effects = _effects;
		layerDepth = _layerDepth;

		shouldRender = true;
	}
}
