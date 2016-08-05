
#include "PrintConsole.h"


#include "../EDRendererD3D/TextRenderer.h"
using namespace DirectX;

namespace EDUtilities
{
	static const wchar_t *PageNames[PrintConsole::PPN_Total] = { L"Controls", L"Render", L"Input", L"Solutions", L"Memory", L"Commands", L"Selection"};
	static const wchar_t *ClassNames[NUM_CLASSES] = { L"ED2", L"ED3", L"ED1" };

	PrintConsole &PrintConsole::GetReference()
	{
		static PrintConsole console;
		return console;
	}

	void PrintConsole::DeleteInstance()
	{
	}

	PrintConsole::PrintConsole(void)
	{
		isEnabled = false;
		currentPage = PPN_Controls;

		currentClassNo = 0;
		currentLabNo = 0;

		Initialize();
	}

	PrintConsole::~PrintConsole(void)
	{
	}

	void PrintConsole::Initialize(void)
	{
		wchar_t buffer[128];
		for(PrintPageNames i = (PrintPageNames)0; i < PPN_Total; i = (PrintPageNames)(i + 1))
		{
			swprintf_s(buffer, 128, L"%s %d/%d", PageNames[i], i+1, PPN_Total);
			pages[i].SetPageTitle(buffer);
		}

		pages[PPN_ED1Solutions].AddClass(ClassNames[0]);
		pages[PPN_ED1Solutions].AddClass(ClassNames[1]);
		pages[PPN_ED1Solutions].AddClass(ClassNames[2]);
	}

	void PrintConsole::Render(SpriteFont *spriteFontPtr, SpriteBatch *spriteBatchPtr, 
		const bool bRunSpriteBatch)
	{
		if(!isEnabled)
			return;

		if(currentPage != PPN_ED1Solutions)
		{
			pages[currentPage].Render();
		}
		else
		{
			pages[PPN_ED1Solutions].RenderSolutions( 
				ClassNames[currentClassNo], currentLabNo);
		}

		if(bRunSpriteBatch)
			spriteBatchPtr->Begin();
		EDRendererD3D::TextRenderer::GetReference().Render(spriteFontPtr, spriteBatchPtr);
		if(bRunSpriteBatch)
			spriteBatchPtr->End();

		if(currentPage != PPN_ED1Solutions)
		{
			pages[currentPage].ClearStrings();
		}
	}

	void PrintConsole::ToggleConsoleCallBack(void * evnt, void * comp)
	{
		PrintConsole &console = PrintConsole::GetReference();
		console.isEnabled = !console.isEnabled;
	}

	void PrintConsole::NextPageCallBack(void * evnt, void * comp)
	{
		PrintConsole &console = PrintConsole::GetReference();

		console.currentPage = ((PrintPageNames)(console.currentPage + 1)); 
		if(console.currentPage >= PPN_Total)
			console.currentPage = (PrintPageNames)(console.currentPage - PPN_Total);
	}

	void PrintConsole::PreviousPageCallBack(void * evnt, void * comp)
	{
		PrintConsole &console = PrintConsole::GetReference();
		console.currentPage = ((PrintPageNames)(console.currentPage - 1)); 
		if(console.currentPage < 0)
			console.currentPage = (PrintPageNames)(console.currentPage + PPN_Total);
	}

	void PrintConsole::NextClassCallBack(void * evnt, void * comp)
	{
		PrintConsole &console = PrintConsole::GetReference();
		if(++console.currentClassNo >= NUM_CLASSES)
			console.currentClassNo = 0;

		console.currentLabNo = 0;
	}

	void PrintConsole::NextLabCallBack(void * evnt, void * comp)
	{
		PrintConsole &console = PrintConsole::GetReference();
		if(++console.currentLabNo >= 
			(unsigned int)console.pages[PPN_ED1Solutions].GetNumSolutions(
			ClassNames[console.currentClassNo]))
		{
			console.currentLabNo = 0;
		}
	}

}