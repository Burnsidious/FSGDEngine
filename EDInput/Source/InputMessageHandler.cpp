#include "./InputMessageHandler.h"
#include "./MouseState.h"

#include "../../EDUtilities/InternalOutput.h"

namespace EDInput
{
	extern HWND hWnd;
	extern Mouse::MouseState mouseState;
	extern InputMode inputMode;

	namespace MessageHandler
	{
		void SetCursorEvent(unsigned int _msg, const void * _data, void *)
		{
			UNREFERENCED_PARAMETER(_msg);

			if (nullptr == _data)
			{
				return;
			}

			POINT mouse = *static_cast<const POINT *>(_data);
			POINT curMouse = *mouseState.GetMousePosition();

			//Don't send another mouse move message if there is nothing to do
			if (0 == memcmp(&mouse, &curMouse, sizeof(POINT) ) )
			{
				return;
			}

			ClientToScreen(hWnd, &mouse);
			SetCursorPos(mouse.x, mouse.y);
			mouseState.SetMousePosition(&mouse);
		}
		void ShowCursorEvent(unsigned int _msg, const void * _data, void *)
		{
			UNREFERENCED_PARAMETER(_msg);

			if (nullptr == _data)
			{
				return;
			}
			bool state = *static_cast<const bool *>(_data);

			if (mouseState.IsShowMouse() != state)
			{
				mouseState.SetMouseShow(state);

				return;
			}

			EDUtilities::InternalOutput::GetReference() << "In " << __FILE__ << __LINE__ << " Setting Show mouse state multiple times\n";
		}
		void ToggleInputCallBack(unsigned int _msg, const void *, void *)
		{
			UNREFERENCED_PARAMETER(_msg);

			inputMode = (TYPED_INPUT == inputMode ? BASIC_INPUT : TYPED_INPUT);
		}
		void SetInputMode(unsigned int _msg, const void *_data, void *)
		{ 
			if (nullptr == _data)
			{
				return;
			}

			InputMode mode = *static_cast<const InputMode *>(_data);
			inputMode = mode; 
		}
	};
};