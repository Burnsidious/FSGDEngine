#include "PrintPage.h"


using namespace DirectX;
using namespace EDRendererD3D;
using std::list;
using std::string;
using std::pair;
namespace EDUtilities
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	// PrintPage::PrintSolution
	//PrintPage::PrintSolution::PrintSolution(void)
	//{
	//	numLabs = 0;
	//}

	PrintPage::PrintSolution::PrintSolution(const wchar_t *_string)
	{
		//numLabs = _numLabs;
		classTitle.text = _string;
	}

	PrintPage::PrintSolution::~PrintSolution(void)
	{
		Destroy();
	}

	void PrintPage::PrintSolution::Destroy(void)
	{
		//std::list< std::list<RenderString *> > solutions;
		auto solutionsIter = solutions.begin();
		auto solutionsIterEnd = solutions.end();
		for(; solutionsIter != solutionsIterEnd; ++solutionsIter)
		{
			auto stringsIter = (*solutionsIter).second.begin();
			auto stringsIterEnd = (*solutionsIter).second.end();

			for(; stringsIter != stringsIterEnd; ++stringsIter)
			{
				delete (*stringsIter);
			}
			(*solutionsIter).second.clear();
		}
		solutions.clear();
	}

	PrintPage::PrintSolution::PrintSolution(const PrintSolution &ps)
	{
		*this = ps;
	}

	PrintPage::PrintSolution::PrintSolution(PrintSolution &&ps)
	{
		*this = std::move(ps);
	}

	PrintPage::PrintSolution &PrintPage::PrintSolution::operator=(const PrintSolution &rps)
	{
		if(this != &rps)
		{
			classTitle = rps.classTitle;
			//numLabs = rps.numLabs;
			solutions = rps.solutions;
		}
		return *this;
	}

	PrintPage::PrintSolution &PrintPage::PrintSolution::operator=(PrintSolution &&rps)
	{
		if(this != &rps)
		{
			classTitle = std::move(rps.classTitle);
			//numLabs = std::move(rps.numLabs);
			solutions = std::move(rps.solutions);
		}
		return *this;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// PrintPage
	PrintPage::PrintPage(void)
	{
		yRasterUpdate = 32/768.0f;
		yRasterUpdate = 20;
		numStrings = 0;
	}

	PrintPage::~PrintPage(void)
	{
		Destroy();
	}

	void PrintPage::Destroy(void)
	{
		numStrings = 0;

		size_t solSize = classSolutions.size();
		for(size_t solIter = 0; solIter < solSize; ++solIter)
			delete classSolutions[solIter];
		classSolutions.clear();
	}

	void PrintPage::SetPageTitle(const wchar_t *_pageTitle)
	{
		pageTitle.text = _pageTitle;
	}

	void PrintPage::Render()
	{
		XMFLOAT3 position(0, 0, 0);

		pageTitle.QueueRender();

		position.y += yRasterUpdate;
		for(size_t i = 0; i < numStrings; ++i)
		{
			position.y += yRasterUpdate;

			strings[i].position = position;
			strings[i].QueueRender();
		}
	}

	void PrintPage::ClearStrings() 
	{ 
		numStrings = 0;
	}

	size_t PrintPage::AddClass(wchar_t const *_string)
	{
		// Loop through and find the current index it resides.
		size_t const uNumClasses = classSolutions.size();
		for(unsigned int uClass = 0; uClass < uNumClasses; ++uClass)
			if(classSolutions[uClass]->classTitle.text == _string)
				return uClass;

		classSolutions.push_back(new PrintSolution(_string));

		size_t const uCurrent = classSolutions.size()-1;

		return uCurrent;
	}
	void PrintPage::AddSolution(const wchar_t *szClass, std::string labName, EDMath::Float3 _color, 
			const wchar_t *_string)
	{
		AddSolution(szClass, labName, XMFLOAT4(_color.x, _color.y, _color.z, 1),
			_string);
	}

	void PrintPage::AddSolution(const wchar_t *szClass, std::string labName, XMFLOAT3 _color, 
		const wchar_t *_string)
	{
		AddSolution(szClass, labName, XMFLOAT4(_color.x, _color.y, _color.z, 1), _string);
	}
	void PrintPage::AddSolution(const wchar_t *szClass, std::string labName, XMFLOAT4 color, 
		const wchar_t *_string)
	{
		//return;
		if(NULL == _string)
			return;

		for(unsigned int uClass = 0; uClass < NUM_CLASSES; ++uClass)
		{
			if(szClass == classSolutions[uClass]->classTitle.text)
			{
				//if(labIndex < 0 || (unsigned int)labIndex >= classSolutions[uClass]->solutions.size())
				//	return;
				
				auto labPage = classSolutions[uClass]->solutions.find(labName);

				// Test if this page already exists
				if(classSolutions[uClass]->solutions.end() == labPage)
				{
					// Add the new page
					const unsigned int bufferSize = 64;
					wchar_t buffer[bufferSize] = {0};
					wchar_t labNameBuffer[bufferSize] = {0};

					mbstowcs_s(nullptr, labNameBuffer, labName.c_str(), bufferSize);

					swprintf_s(buffer, L"Lab: %s", labNameBuffer);
					RenderString *stringPtr = new RenderString;
					stringPtr->text = buffer;

					list<EDRendererD3D::RenderString *> labStrings;
					labStrings.push_back(stringPtr);

					classSolutions[uClass]->solutions.insert(
						pair<string, list<EDRendererD3D::RenderString *> >(
							labName, labStrings));

					// The page should exist now
					labPage = classSolutions[uClass]->solutions.find(labName);
				}
				
				RenderString *stringPtr = new RenderString;
				stringPtr->text = _string;
				stringPtr->color = color;
				(*labPage).second.push_back(stringPtr);
			}
		}
	}

	void PrintPage::RenderSolutions(const wchar_t *_string, int const labIndex)
	{
		XMFLOAT3 position(0, 0, 0);
		
		pageTitle.QueueRender();

		position.y += yRasterUpdate;

		if(NULL == _string)
			return;

		for(unsigned int uClass = 0; uClass < NUM_CLASSES; ++uClass)
		{
			if(_string == classSolutions[uClass]->classTitle.text)
			{
				if(labIndex < 0 || (unsigned int)labIndex >= classSolutions[uClass]->solutions.size())
					return;

				classSolutions[uClass]->classTitle.position = position;
				classSolutions[uClass]->classTitle.QueueRender();

				auto solutionsIter = 
					classSolutions[uClass]->solutions.begin();
				for(int i = 0; i < labIndex; ++i)
					++solutionsIter;

				auto stringIter = (*solutionsIter).second.begin();
				auto stringIterEnd = (*solutionsIter).second.end();

				for(; stringIter != stringIterEnd; ++stringIter)
				{
					position.y += yRasterUpdate;

					(*stringIter)->position = position;
					(*stringIter)->QueueRender();
				}
			}
		}
	}

	size_t PrintPage::GetNumSolutions(const wchar_t *_string) const
	{
		for(unsigned int uClass = 0; uClass < NUM_CLASSES; ++uClass)
			if(_string == classSolutions[uClass]->classTitle.text)
				return classSolutions[uClass]->solutions.size();

		return -1;
	}
}