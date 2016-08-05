#ifndef _SETTINGS_H_
#define _SETTINGS_H_

// Currently just a wrapper for some xml calls, attempt to make the engine more data driven
// each class can now have its own level and any other settings needed

#include <string>

#include "../tinyxml/tinyxml.h"

using namespace std;

namespace EDUtilities
{
	class Settings
	{
	public:

		static Settings* GetInstance();

		// INPUT _file - name of XML to load
		// RETURN if the load was success will return true
		bool LoadFromXML(char* _file);

		// INPUT	_tag		xml tag element to look for
		// OUTPUT	_output		data found at tag
		// INPUT	_default	what to return to user if nothing is found or document not loaded
		// RETURN	if nothing was found or document not load will return false, otherwise true
		bool GetSetting(const char* _tag, string& _output, const char* _default = "default");
		bool GetSetting(const char* _tag, float& _output, float _default = 0.0f);
		bool GetSetting(const char* _tag, int& _output, int _default = 0);
		bool GetSetting(const char* _tag, unsigned int& _output, unsigned int _default = 0);
		bool GetSetting(const char* _tag, bool& _output, bool _default = false);

	private:

		Settings();
		~Settings();

		bool is_open;
		TiXmlDocument file;	
	};
}

#endif
