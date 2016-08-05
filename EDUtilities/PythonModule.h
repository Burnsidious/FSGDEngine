#ifndef _PYTHON_INTERFACE_H_
#define _PYTHON_INTERFACE_H_

/*  Interface to a Python module

	If you need to print out to the console you will need to do the following once
	in a script, sys.stdout = open('CONOUT$', 'w'), currently in engine_script
	This will make sure any output gets redirected to our Internal Output console
*/

#include <unordered_map>
#include <string>

#include "../Python/Python.h"

using namespace std;

namespace EDUtilities
{
	class PythonModule
	{
	private:
		PyObject* module;
		PyObject* dictionary;
		string module_name;

	public:

		bool IsValid() { return module != nullptr; }
		string GetModuleName() { return module_name;  }

		// inializing Python interpeter is trivial, didn't want to create an extra manager to do so
		static void Initialize();
		static void Shutdown();

		PythonModule() : module(nullptr), dictionary(nullptr) {}
		~PythonModule() { UnloadModule(); }

		// calls LoadModule function with passed string
		// INPUT _name - name of the module to load (omit the extension)
		PythonModule(string _name);

		// prepares a module 
		// INPUT _name - name of the module to load (omit the extension)
		// RETURN - if the module was successfully loaded or not
		bool LoadModule(string _filename);
		bool LoadModuleSolution(string _filename);

		void ReloadModule();
		void ReloadModuleSolution();

		// cleans up any existing python objects 
		void UnloadModule();
		void UnloadModuleSolution();
		
		// look up and call python function by name
		// INPUT _name - name of function to call
		void CallFunction(string _name);
		void CallFunctionSolution(string _name);

		// set a global python value, be careful of implicit string conversion
		// lazyness permitting try to use string("") over ""
		// INPUT _name - name of global variable to set
		// INPUT _value - what to set the python value to
		void SetValue(string _name, int _value);		
		void SetValue(string _name, float _value);
		void SetValue(string _name, string _value);
		void SetValue(string _name, bool _value);
		void SetValueSolution(string _name, int _value);
		void SetValueSolution(string _name, float _value);
		void SetValueSolution(string _name, string _value);
		void SetValueSolution(string _name, bool _value);

		// retrieves a global python value
		// INPUT _name - name of global variable to retrieve
		// OUTPUT _value - reference to the retrieved value
		void GetValue(string _name, int& _value);
		void GetValue(string _name, float& _value);
		void GetValue(string _name, string& _value);
		void GetValue(string _name, bool& _value);
		void GetValueSolution(string _name, int& _value);
		void GetValueSolution(string _name, float& _value);
		void GetValueSolution(string _name, string& _value);
		void GetValueSolution(string _name, bool& _value);
	};
}

#endif
