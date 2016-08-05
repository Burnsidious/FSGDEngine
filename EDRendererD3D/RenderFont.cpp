#include "precompiled.h"
#include "RenderFont.h"
#include "Renderer.h"

#include "../EDUtilities/InternalOutput.h"
using EDUtilities::InternalOutput;

using std::ifstream;

namespace EDRendererD3D
{
	RenderFont::RenderFont()
	{
		charHeight = 20;
	}

	RenderFont::~RenderFont()
	{
		Destroy();
	}

	void RenderFont::Destroy()
	{
	}

	RenderFont *RenderFont::Load(const char *fontMetricsFileName, const  char *textureFileName)
	{
		RenderFont *fontPtr = new RenderFont();
		
		fontPtr->Destroy();

		if(!fontPtr->LoadFontMetrics(fontMetricsFileName))
			return false;

		// Load the texture
		WCHAR *buffer = new WCHAR[strlen(textureFileName)+1];

		unsigned int length = static_cast<unsigned int>(strlen(textureFileName));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, textureFileName, 
			length, buffer, length);

		buffer[strlen(textureFileName)] = 0;
		fontPtr->texture.Create(buffer, "Font");

		delete[] buffer;

		return fontPtr;
	}

	bool RenderFont::LoadFontMetrics(const char *filename)
	{
		ifstream fin;
		char temp;
		fin.open(filename);

		if(fin.fail())
		{
			InternalOutput::GetReference().Error("RenderFont::LoadFontMetrics Failed to open file named %s", filename);
			return false;
		}

		// Read in the metrics
		for(unsigned int index = 0; index < NUM_CHARACTERS; ++index)
		{
			// Skip the ascii value
			fin.get(temp);
			while(temp != ' ')
			{
				fin.get(temp);
			}
			
			// Skip the character value
			fin.get(temp);
			while(temp != ' ')
			{
				fin.get(temp);
			}

			// Store the metrics we will use
			fin >> fontMetrics[index].left;
			fin >> fontMetrics[index].right;
			fin >> fontMetrics[index].size;
		}

		fin.close();

		return true;
	}
}