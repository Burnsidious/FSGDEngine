#pragma once



#include "RenderString.h"
#include "Renderer.h"

namespace EDRendererD3D
{
	class TextRenderer
	{
	public:
		~TextRenderer();

		static TextRenderer *GetInstance();
		static TextRenderer &GetReference();
		static void DeleteInstance();

		/// Renders the contained RenderString instances
		/// \param spriteBatchPtr - Pointer to the SpriteBatch instance to use for rendering
		/// \param bRunSpriteBatch - If true calls begin and end on the sprite batch, pass false 
		/// when calling this method between a begin and end sprite batch block
		void Render(DirectX::SpriteFont *spriteFontPtr, DirectX::SpriteBatch *spriteBatchPtr, 
			const bool bRunSpriteBatch = false);

		/// Adds a RenderString instance to be rendered when the Render method of this 
		/// class is called
		/// \param renderStringPtr - Pointer to the RenderString instance to queue for rendering
		void AddRenderString(RenderString *renderStringPtr);
	private:

		TextRenderer();
		TextRenderer(const TextRenderer &){}
		TextRenderer &operator=(const TextRenderer &) {}

		static TextRenderer *instancePtr;

		std::list<RenderString *> stringsToBeRendered;
	};
}