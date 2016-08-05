#pragma once



namespace DirectX
{
	class SpriteBatch;
}

namespace EDRendererD3D
{
	class RenderSpriteManager
	{
		friend class RenderSprite;
	public:
		~RenderSpriteManager(void);

		static RenderSpriteManager &GetReference();

		static void DeleteInstance();

		void Render(DirectX::SpriteBatch *spriteBatchPtr);

	private:
		RenderSpriteManager(void);
		RenderSpriteManager(const RenderSpriteManager &) {}
		RenderSpriteManager &operator=(const RenderSpriteManager &) { return *this; }

		static RenderSpriteManager *instancePtr;

		void AddSprite(RenderSprite *spritePtr);
		void RemoveSprite(RenderSprite *spritePtr);
		std::list<RenderSprite *> sprites;
	};
}
