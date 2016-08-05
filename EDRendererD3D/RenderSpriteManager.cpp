#include "precompiled.h"
#include "RenderSpriteManager.h"
#include "RenderSprite.h"
#include "../DirectXTK/Inc/SpriteBatch.h"

using DirectX::SpriteBatch;

namespace EDRendererD3D
{
	RenderSpriteManager *RenderSpriteManager::instancePtr = 0;


	RenderSpriteManager::RenderSpriteManager(void)
	{
	}


	RenderSpriteManager::~RenderSpriteManager(void)
	{
	}

	RenderSpriteManager &RenderSpriteManager::GetReference()
	{
		if(!instancePtr)
		{
			instancePtr = new RenderSpriteManager;
		}
		return *instancePtr;
	}

	void RenderSpriteManager::DeleteInstance()
	{
		delete instancePtr;
		instancePtr = nullptr;
	}

	void RenderSpriteManager::Render(SpriteBatch *spriteBatchPtr)
	{
		std::list<RenderSprite *>::iterator iter = sprites.begin();
		std::list<RenderSprite *>::iterator iterEnd = sprites.end();

		RenderSprite *spritePtr = nullptr;
		for(; iter != iterEnd; ++iter)
		{
			spritePtr = (*iter);
			if(spritePtr->GetShouldRender())
			{
				spriteBatchPtr->Draw(spritePtr->textureSRV, spritePtr->destinationRect, 
					spritePtr->sourceRect, XMLoadFloat4(&spritePtr->color), spritePtr->rotation, 
					spritePtr->origin, spritePtr->effects, spritePtr->layerDepth);
			}
		}
	}

	void RenderSpriteManager::AddSprite(RenderSprite *spritePtr)
	{
		sprites.push_back(spritePtr);
	}

	void RenderSpriteManager::RemoveSprite(RenderSprite *spritePtr)
	{
		sprites.remove(spritePtr);
	}
}
