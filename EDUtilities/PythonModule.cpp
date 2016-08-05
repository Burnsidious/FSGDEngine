#include "PythonModule.h"
#include "InternalOutput.h"

namespace EDUtilities
{
	void PythonModule::Initialize()
	{
		// internal output must be created before python inits
		InternalOutput::GetReference();

		// tells python where to look for its 'lib' files
		Py_NoSiteFlag=1;
		Py_SetPythonHome(L"../Python/.");
		Py_Initialize();
	
		// tell python where to search for modules
		std::wstring new_path = Py_GetPath();
		new_path += L";../Scripts";
		PySys_SetPath(new_path.c_str());
	}

	void PythonModule::Shutdown()
	{
		Py_Finalize();
	}

	void PythonModule::ReloadModule()
	{
		ReloadModuleSolution();
	}

	PythonModule::PythonModule(string _name)
	{
		// TODO - comment this out and write your own solution
		LoadModule(_name);
	}

	bool PythonModule::LoadModule(string _filename)
	{
		// TODO - comment this out and write your own solution
		bool success = false;
		success = LoadModuleSolution(_filename);
		return success;
	}

	void PythonModule::UnloadModule()
	{
		// TODO - comment this out and write your own solution
		UnloadModuleSolution();
	}

	void PythonModule::CallFunction(string _name)
	{
		// TODO - comment this out and write your own solution
		CallFunctionSolution(_name);
	}

	void PythonModule::SetValue(string _name, int _value)
	{
		// TODO - comment this out and write your own solution
		SetValueSolution(_name, _value);
	}
	
	void PythonModule::SetValue(string _name, float _value)
	{
		// TODO - comment this out and write your own solution
		SetValueSolution(_name, _value);
	}

	void PythonModule::SetValue(string _name, string _value)
	{
		// TODO - comment this out and write your own solution
		SetValueSolution(_name, _value);
	}

	void PythonModule::SetValue(string _name, bool _value)
	{
		// TODO - comment this out and write your own solution
		SetValueSolution(_name, _value);
	}
	
	void PythonModule::GetValue(string _name, int& _value)
	{
		// TODO - comment this out and write your own solution
		GetValueSolution(_name, _value);
	}

	void PythonModule::GetValue(string _name, float& _value)
	{
		// TODO - comment this out and write your own solution
		GetValueSolution(_name, _value);
	}

	void PythonModule::GetValue(string _name, string& _value)
	{
		// TODO - comment this out and write your own solution
		GetValueSolution(_name, _value);
	}

	void PythonModule::GetValue(string _name, bool& _value)
	{
		// TODO - comment this out and write your own solution
		GetValueSolution(_name, _value);
	}
}
