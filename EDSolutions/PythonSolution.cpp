#include "../EDUtilities/PythonModule.h"
#include "../EDUtilities/InternalOutput.h"

namespace EDUtilities
{
	bool PythonModule::LoadModuleSolution(string _filename)
	{
		bool success = false;
		module_name = _filename;
		module = PyImport_ImportModule(module_name.c_str());
		if (module != nullptr)
		{
			dictionary = PyModule_GetDict(module);
			success = true;
		}
		else
		{
			InternalOutput::GetReference().Error("Python Error Module (File): %s not found.\n", module_name.data());
		}
		return success;
	}

	void PythonModule::UnloadModuleSolution()
	{
		if (module)
		{
			Py_DECREF(module);
			module = nullptr;
			dictionary = nullptr;
		}
	}

	void PythonModule::ReloadModuleSolution()
	{
		if (module != nullptr)
		{
			module = PyImport_ReloadModule(module);
			if (module != nullptr)
			{
				dictionary = PyModule_GetDict(module);
			}
		}
	}

	void PythonModule::CallFunctionSolution(string _name)
	{
		PyObject* func = PyDict_GetItemString(dictionary, _name.c_str());
		if (func)
		{
			PyObject_CallObject(func, nullptr);
		}
	}

	void PythonModule::SetValueSolution(string _name, int _value)
	{
		PyObject* py_value = PyLong_FromLong(_value);
		PyDict_SetItemString(dictionary, _name.c_str(), py_value);
		Py_DECREF(py_value);
	}

	void PythonModule::SetValueSolution(string _name, float _value)
	{
		PyObject* py_value = PyFloat_FromDouble(_value);
		PyDict_SetItemString(dictionary, _name.c_str(), py_value);
		Py_DECREF(py_value);
	}

	void PythonModule::SetValueSolution(string _name, string _value)
	{
		PyObject* py_value = PyUnicode_FromString(_value.c_str());
		PyDict_SetItemString(dictionary, _name.c_str(), py_value);
		Py_DECREF(py_value);
	}

	void PythonModule::SetValueSolution(string _name, bool _value)
	{
		PyObject* py_value = PyBool_FromLong(static_cast<long>(_value));
		PyDict_SetItemString(dictionary, _name.c_str(), py_value);
		Py_DECREF(py_value);
	}

	void PythonModule::GetValueSolution(string _name, int& _value)
	{
		PyObject* ret = PyDict_GetItemString(dictionary, _name.c_str());
		_value = PyLong_AsLong(ret);
	}

	void PythonModule::GetValueSolution(string _name, float& _value)
	{
		PyObject* ret = PyDict_GetItemString(dictionary, _name.c_str());
		_value = static_cast<float>(PyFloat_AsDouble(ret));
	}

	void PythonModule::GetValueSolution(string _name, string& _value)
	{
		PyObject* ret = PyDict_GetItemString(dictionary, _name.c_str());
		_value = PyUnicode_AsUTF8(ret);
	}

	void PythonModule::GetValueSolution(string _name, bool& _value)
	{
		PyObject* ret = PyDict_GetItemString(dictionary, _name.c_str());
		_value = (PyObject_IsTrue(ret) == 1) ? true : false;
	}
}
