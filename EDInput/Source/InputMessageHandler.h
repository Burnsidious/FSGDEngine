#pragma once

//Collection of bound functions to the event system
namespace EDInput
{
	namespace MessageHandler
	{
		//Event: "Set Cursor"
		//_data type is POINT *
		void SetCursorEvent(unsigned int _msg, const void * _data, void *);

		//Event: "Show Cursor"
		//_data type is bool *
		void ShowCursorEvent(unsigned int _msg, const void * _data, void *);

		//Event: "Toggle Input"
		void ToggleInputCallBack(unsigned int _msg, const void *, void *);

		//Event: "Set Input"
		//_data type is bool *
		void ShowCursorEvent(unsigned int _msg, const void * _data, void *);

		//Event: "Set Input"
		//_data type is bool *
		void SetInputMode(unsigned int _msg, const void *_data, void *);
	};
};