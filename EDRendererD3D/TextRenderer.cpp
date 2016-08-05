#include "precompiled.h"
#include "TextRenderer.h"
#include "Renderer.h"

using namespace DirectX;
using namespace std;

namespace EDRendererD3D
{
	TextRenderer *TextRenderer::instancePtr = 0;

	TextRenderer::TextRenderer()
	{
	}

	TextRenderer::~TextRenderer()
	{
	}

	TextRenderer *TextRenderer::GetInstance()
	{
		if(!instancePtr)
		{
			instancePtr = new TextRenderer;
		}
		return instancePtr;
	}
	
	TextRenderer &TextRenderer::GetReference()
	{
		return *GetInstance();
	}

	void TextRenderer::DeleteInstance()
	{
		delete instancePtr;
	}

	void TextRenderer::AddRenderString(RenderString *renderStringPtr) 
	{
		// Add the string to the container to be rendered later
		stringsToBeRendered.push_back(renderStringPtr);
	}

	void TextRenderer::Render(SpriteFont *spriteFontPtr, SpriteBatch *spriteBatchPtr,
		const bool bRunSpriteBatch)
	{
		if(bRunSpriteBatch)
			spriteBatchPtr->Begin();
		list<RenderString *>::iterator containerIter = stringsToBeRendered.begin();
		list<RenderString *>::iterator containerIterEnd = stringsToBeRendered.end();

		for(; containerIter != containerIterEnd; ++containerIter)
		{
			(*containerIter)->Render(spriteFontPtr, spriteBatchPtr);
		}
		stringsToBeRendered.clear();

		if(bRunSpriteBatch)
			spriteBatchPtr->End();
	}
}