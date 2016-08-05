#pragma once
#include "../EDRendererD3D/RenderString.h"

#include "../EDMath/Float3.h"

#define NUM_CLASSES 3

namespace EDUtilities
{
    /// A single page of text to be printed by the PrintConsole.
    /// \sa PrintConsole
	class PrintPage
	{
	private:

		class PrintSolution
		{
			friend PrintPage;
		private:
			/// Class
			EDRendererD3D::RenderString classTitle;
			/// Number of labs for this class.
			//unsigned int numLabs;
			/// Vector of strings to be rendered for each lab.
			std::map< std::string, std::list<EDRendererD3D::RenderString *> > solutions;

			PrintSolution(void);
		public:
			/// Constructor sets variables to default state.
			//
			PrintSolution(const wchar_t *sz);
			/// Copy constructor creates a new PrintSolution based on an existing one.
			/// \param ps - The PrintSolution to copy from.
			PrintSolution(const PrintSolution &ps);
			PrintSolution( PrintSolution &&ps);
			/// Copies one PrintSolution to another one.
            /// \param rps - The PrintString to copy from.
            /// \return Returns the result of the assignment.
            PrintSolution &operator=(const PrintSolution &rps);
			/// Moves one PrintSolution to another one.
            /// \param rps - The PrintString to copy from.
            /// \return Returns the result of the assignment.
            PrintSolution &operator=( PrintSolution &&rps);
            /// Cleans up the dynamic string.
			~PrintSolution(void);

			void Destroy(void);
            /// Sets the string to be printed.
            /// \param sz - A null terminated C style string we want to copy from.
			void SetClassTitle(const wchar_t *_classTitle);
		};

		/// The maximum number of strings this PrintPage supports.
		static const size_t MaxStrings = 64;
		/// The number of strings added to this page.
		size_t numStrings;
        /// The amount of space desired between new strings printed.
		float yRasterUpdate;
        /// The title of this page.  This will be printed first.
		EDRendererD3D::RenderString pageTitle;
        /// The strings to be printed on this page. Only indices less than numStrings are valid.
		EDRendererD3D::RenderString strings[MaxStrings];

		/// The lab solutions for each class
		std::vector<PrintSolution *> classSolutions;
	public:
		PrintPage(void);
		~PrintPage(void);

        /// The maximum number of characters to be printed for a single string.
		static const int MaxStringLength = 128;
        /// Sets the title for this page
        /// \param sz - The string to set the title to
		void SetPageTitle(const wchar_t *_pageTitle);
        /// Adds a new string to be printed on this page this frame.
        /// \param color - The color the string should be printed in
        /// \param sz - The string to print
		template <typename T>
		void AddString(T &&_color, const wchar_t *_string)
		{
			assert(numStrings < MaxStrings);

			strings[numStrings].SetColor(_color);
			strings[numStrings].text = _string;

			numStrings++;
		}

        /// Renders this page
        void Render();
        /// Deletes all the strings added so far.  This method is called each frame.
		void ClearStrings();

		void Destroy(void);

		size_t AddClass(wchar_t const *_string);

		void AddSolution(const wchar_t *szClass, std::string labName, EDMath::Float3 _color, 
			const wchar_t *_string);
		void AddSolution(const wchar_t *szClass, std::string labName, DirectX::XMFLOAT3 _color, 
			const wchar_t *_string);
		void AddSolution(const wchar_t *szClass, std::string labName, DirectX::XMFLOAT4 _color, 
			const wchar_t *_string);
		void RenderSolutions(const wchar_t *_string, int const labIndex);
		size_t GetNumSolutions(const wchar_t *_string) const;
	};
}
