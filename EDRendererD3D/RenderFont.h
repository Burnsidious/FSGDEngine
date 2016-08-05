#pragma once



#include "RenderTexture2D.h"
#include "VertexFormats.h"

namespace EDRendererD3D
{
	class RenderFont
	{
	public:
		struct RenderFontMetric
		{
			float	left;
			float	right;
			int		size;
		};
		RenderFont();
		~RenderFont();
		
		static RenderFont *Load(const char *fontMetricsFileName, const  char *textureFileName);

		void Destroy();

		inline ID3D11ShaderResourceView *GetShaderResourceView()
		{
			return texture.GetShaderResourceView();
		}

		inline const RenderFontMetric &GetFontMetric(unsigned int i) { return fontMetrics[i]; }
		inline const unsigned int GetCharHeight() { return charHeight; }
	private:

		/// This is the number of characters in the font asset I have currently
		/// If this class sees heavy use this should likely become dynamic and be driven by the asset.
		static const int NUM_CHARACTERS = 95;
		RenderFontMetric fontMetrics[NUM_CHARACTERS];
		RenderTexture2D texture;

		/// This will define how tall each quad a character is rendered to will be
		unsigned int charHeight;
		bool LoadFontMetrics(const char *filename);
	};
}