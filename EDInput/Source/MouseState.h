#pragma once

#include "../InputManager.h"
#include "./InternalEnums.h"

namespace EDInput
{
	namespace Mouse
	{
		// This class holds data about the current state of the mouse.
		class MouseState
		{
		public:
			MouseState();
			void Initialize(HWND _hWnd);
			void clear();

			//updates the value of the wheel
			int16_t ChangeWheelData(int16_t _movement);

			bool SetState(MOUSE_BUTTON _button, ButtonState _state);
			void SetMousePosition(const POINT * _pos);
			void SetMouseFocus(bool _state);
			void SetMouseShow(bool _state);

			bool IsMouseInFocus();
			bool IsShowMouse();
			ButtonState IsPressed(MOUSE_BUTTON _button);

			const POINT * GetMousePosition();
			int16_t GetWheelData();
		private:
			//Windows event sending extra messages to windproc
			TRACKMOUSEEVENT mouse_event;

			ButtonState	State[MAX_BUTTONS];
			POINT		CurrentMousePos;
			int16_t		WheelData;
			bool		MouseinFocus;
			bool		ShowMouse;
		};
	};
}