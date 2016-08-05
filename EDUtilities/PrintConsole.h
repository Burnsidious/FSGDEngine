#pragma once

#include "PrintPage.h"

namespace EDUtilities
{
    /// An internal console implementation.
    /// This class can be used to print text to screen for debug output.
	class PrintConsole
	{
	public:
        /// This enum represents the pages of text to printed to the screen.
        /// If a new page is added, a new name will need to be added to the static string arrat
        /// PageNames in the source file.
		enum PrintPageNames{PPN_Controls = 0, PPN_Render, PPN_Input, PPN_ED1Solutions, PPN_Memory, PPN_COMMANDS, PPN_Selection, PPN_Total};
	private:
        /// Private constructor sets parameters to default state and ensures singleton structure.
		PrintConsole(void);
        /// Private copy constructor ensures singleton structure.
        PrintConsole(const PrintConsole&) {}
        /// Private assignment operator ensures singleton structure.
        PrintConsole &operator=(const PrintConsole &) {}

		/// The pages of text that can be printed.
		PrintPage pages[PPN_Total];

        /// Flag used to decide if text pages should be rendered.
		bool isEnabled;

        /// The page that should be rendered.
		PrintPageNames currentPage;
		
		/// The class that will render if the solutions page is up.
		unsigned int currentClassNo;
		
		/// The current lab that will render for the specific class.
		unsigned int currentLabNo;

	public:
        /// Gets a refernce to the PrintConsole.
		static PrintConsole &GetReference();
        /// Destroys the instance of PrintConsole.
        static void DeleteInstance();
        /// Initializes the PrintConsole.  Must be called before use.
        /// \param pDevice - The D3D device to be used with this PrintConsole.
		void Initialize(void);
        /// Destructor releases and D3D handles
		~PrintConsole(void);
        /// Renders the current page if the console is enabled.
		void Render(DirectX::SpriteFont *spriteFontPtr, DirectX::SpriteBatch *spriteBatchPtr, const bool bRunSpriteBatch = false);
        /// Adds a string to be printed to a specified page.  Strings are cleared each frame.
        /// \param page - The page this string should be added to.
        /// \param color - The color the string should print int.
        /// \param sz - A format control string, ex. "X = %d".
        /// \sa sprintf
        /// \param ... - Optional arguments to be used in format control string.
		template <typename float3color>
		void AddString(PrintPageNames page, float3color&& color, const wchar_t *sz, ...)
		{
			if(!isEnabled || page != currentPage)
				return;

			va_list ap;
			va_start(ap, sz);

			wchar_t buffer[PrintPage::MaxStringLength];
			_vsnwprintf_s(buffer, PrintPage::MaxStringLength, sz, ap);

			// pass that string off to where it should be stored
			pages[page].AddString(std::forward<float3color>(color), buffer);

			va_end(ap);
		}
        /// Sets the enabled state of the console.
		inline void Enable(bool enable) { isEnabled = enable; }
		inline bool IsEnabled() { return isEnabled; }
        /// Sets the current page to be rendered.
		inline void SetCurrentPage(PrintPageNames page) { currentPage = page; }
		/// Adds the string solution to the current class and the current lab
		/// \param szClass - The class to be added to.
		/// \param labIndex - The lab to be added to.
		/// \param color - The color of the string to be printed in.
		/// \param sz - The string to be rendered.
		template <typename float3color>
		void AddSolution(const wchar_t *szClass, std::string labName, float3color&& color, 
			const wchar_t *sz, ...)
		{
			va_list ap;
			va_start(ap, sz);

			wchar_t buffer[PrintPage::MaxStringLength];
			_vsnwprintf_s(buffer, PrintPage::MaxStringLength, sz, ap);

			// pass that string off to where it should be stored
			pages[PPN_ED1Solutions].AddSolution(szClass, labName, 
				std::forward<Float3>(color), buffer);
			va_end(ap);
		}

		static void ToggleConsoleCallBack(void * evnt, void * comp);
		static void NextPageCallBack(void * evnt, void * comp);
		static void PreviousPageCallBack(void * evnt, void * comp);
		static void NextClassCallBack(void * evnt, void * comp);
		static void NextLabCallBack(void * evnt, void * comp);

	};
}