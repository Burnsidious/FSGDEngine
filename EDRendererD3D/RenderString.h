#pragma once



#include "RenderFont.h"

#include "../EDUtilities/ContentManager.h"
#include "../EDMath/Float3.h"
#include "../DirectXTK/Inc/SpriteFont.h"

namespace EDRendererD3D
{
	class RenderString
	{
	public:
		std::wstring text;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		float rotation;
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT3 origin;
		DirectX::SpriteEffects effects;
		float layerDepth;

		RenderString();

		~RenderString();

		void QueueRender();
		void Render(DirectX::SpriteFont *spriteFontPtr, DirectX::SpriteBatch *spriteBatchPtr);

		void SetColor(const EDMath::Float3 &_color) { color = DirectX::XMFLOAT4(_color.x, _color.y, _color.z, 1); }
		void SetColor(const DirectX::XMFLOAT4 &_color) { color = _color; }
	};
}