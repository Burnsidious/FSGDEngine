#include "Settings.h"

namespace EDUtilities
{
	Settings::Settings()
	{
		is_open = false;
	}
	
	Settings::~Settings()
	{
		if (is_open == true)
		{
			file.Clear();
			is_open = false;
		}
	}

	Settings* Settings::GetInstance()
	{
		static Settings settings;
		return &settings;
	}

	bool Settings::LoadFromXML(char* _file)
	{
		is_open = file.LoadFile(_file);
		return is_open;
	}

	bool Settings::GetSetting(const char* _tag, string& _output, const char* _default)
	{
		TiXmlElement* root = nullptr;
		TiXmlElement* element = nullptr;

		if (is_open == true)
		{
			root = file.FirstChildElement();
			if (root != nullptr)
			{
				element = root->FirstChildElement(_tag);
			}
		}

		_output = (element != nullptr) ? element->GetText() : _default;

		return element != nullptr;

	}

	bool Settings::GetSetting(const char* _tag, float& _output, float _default)
	{
		TiXmlElement* root = nullptr;
		TiXmlElement* element = nullptr;

		if (is_open == true)
		{
			root = file.FirstChildElement();
			if (root != nullptr)
				element = root->FirstChildElement(_tag);
		}

		_output = (element != nullptr) ? static_cast<float>(std::atof(element->GetText())) : _default;

		return element != nullptr;
	}

	bool Settings::GetSetting(const char* _tag, int& _output, int _default)
	{
		TiXmlElement* root = nullptr;
		TiXmlElement* element = nullptr;

		if (is_open == true)
		{
			root = file.FirstChildElement();
			if (root != nullptr)
				element = root->FirstChildElement(_tag);
		}

		_output = (element != nullptr) ? std::atoi(element->GetText()) : _default;

		return element != nullptr;
	}

	bool Settings::GetSetting(const char* _tag, unsigned int& _output, unsigned int _default)
	{
		TiXmlElement* root = nullptr;
		TiXmlElement* element = nullptr;

		if (is_open == true)
		{
			root = file.FirstChildElement();
			if (root != nullptr)
				element = root->FirstChildElement(_tag);
		}

		_output = (element != nullptr) ? std::atoi(element->GetText()) : _default;

		return element != nullptr;
	}

	bool Settings::GetSetting(const char* _tag, bool& _output, bool _default)
	{
		TiXmlElement* root = nullptr;
		TiXmlElement* element = nullptr;

		if (is_open == true)
		{
			root = file.FirstChildElement();
			if (root != nullptr)
				element = root->FirstChildElement(_tag);
		}

		_output = (element != nullptr) ? strcmp(element->GetText(), "true") == 0 : _default;

		return element != nullptr;
	}
}
