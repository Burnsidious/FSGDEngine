#include "precompiled.h"
#include "RenderString.h"
#include "Renderer.h"
#include "VertexFormats.h"
#include "TextRenderer.h"


using namespace DirectX;

namespace EDRendererD3D
{
	RenderString::RenderString()
	{
		text = L"Uninitialized";
		position = XMFLOAT3(0, 0, 0);
		color = XMFLOAT4(1, 1, 1, 1);
		rotation = 0;
		origin = XMFLOAT3(0, 0, 0);
		scale = XMFLOAT3(1, 1, 1);
		effects = SpriteEffects_None;
		layerDepth = 0;
	}

	RenderString::~RenderString() 
	{
	}

	void RenderString::QueueRender() 
	{ 
		TextRenderer::GetReference().AddRenderString(this);
	}

	void RenderString::Render(SpriteFont *spriteFontPtr, SpriteBatch *spriteBatchPtr)
	{
		spriteFontPtr->DrawString(spriteBatchPtr, text.c_str(), XMLoadFloat3(&position), 
			XMLoadFloat4(&color), rotation, XMLoadFloat3(&origin),	XMLoadFloat3(&scale), effects, 
			layerDepth);

	}

}