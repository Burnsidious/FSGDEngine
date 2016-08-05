#include "./MouseState.h"

namespace EDInput
{
	namespace Mouse
	{
		MouseState::MouseState() : WheelData(0), MouseinFocus(false), ShowMouse(true)
		{
			for each (ButtonState &entry in State)
				entry = INPUT_RELEASED;
		}
		void MouseState::Initialize(HWND _hWnd)
		{
			for each (ButtonState &entry in State)
				entry = INPUT_RELEASED;

			MouseinFocus = false;
			ShowMouse = true;

			WheelData = 0;
			mouse_event.cbSize = sizeof(TRACKMOUSEEVENT);
			mouse_event.dwFlags = TME_LEAVE;
			mouse_event.dwHoverTime = 0;
			mouse_event.hwndTrack = _hWnd;
		}
		void MouseState::clear()
		{
			for each (ButtonState &entry in State)
				entry = INPUT_RELEASED;
			SetMouseShow(true);
		}
		int16_t MouseState::ChangeWheelData(int16_t _movement)
		{
			WheelData += _movement;
			return WheelData;
		}
		bool MouseState::SetState(MOUSE_BUTTON _button, ButtonState _state)
		{
			if (MAX_BUTTONS > _button)
			{
				if (MAX_STATES > _state)
				{
					State[_button] = _state;
					return true;
				}
			}

			return false;
		}
		void MouseState::SetMousePosition(const POINT * _pos)
		{
			CurrentMousePos = *_pos;
		}
		void MouseState::SetMouseFocus(bool _state)
		{
			MouseinFocus = _state;
			if (true == MouseinFocus)
			{
				TrackMouseEvent(&mouse_event);
			}
			else
			{
				clear();
			}
		}
		void MouseState::SetMouseShow(bool _state)
		{
			if (_state != ShowMouse)
			{
				ShowMouse = _state;
				int ref = ShowCursor(ShowMouse);
				if (ref == 1)
				{
					int error = 0;
				}
			}
		}
		bool MouseState::IsMouseInFocus()
		{
			return MouseinFocus;
		}
		bool MouseState::IsShowMouse()
		{
			return ShowMouse;
		}
		ButtonState MouseState::IsPressed(MOUSE_BUTTON _button)
		{
			return State[_button];
		}
		const POINT * MouseState::GetMousePosition()
		{
			return &CurrentMousePos;
		}
		int16_t MouseState::GetWheelData()
		{
			return WheelData;
		}
	};
};