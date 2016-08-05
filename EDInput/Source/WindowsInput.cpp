#include "../WindowsInput.h"
#include "./EventHelper.h"
#include "./GamepadInput.h"

namespace EDInput
{
	void Initialize(HWND _hWnd)
	{
		Events::Initialize(_hWnd);
		GamePad::Initialize();
	}
	//Called from a couple messages keys X1 and X2 use the same message
	void GetBrowserKeys(WPARAM _wParam, MOUSE_BUTTON &_button, uint16_t &_vk)
	{
		if (HIWORD(_wParam) & 0x0001)
		{
			_button = MOUSE_X1;
			_vk = VK_XBUTTON1;
		}
		else
		{
			_button = MOUSE_X2;
			_vk = VK_XBUTTON2;
		}
	}

	//called to stop messages
	void Update(float _time)
	{
		GamePad::InputProc(_time);
	}

	//  WM_CHAR				- posts typed letters, both upper and lower case
	//  WM_KEYUP			- posts when a key comes up, only upper case
	//  WM_KEYDOWN			- posts when a key comes up, only upper case
	//  WM_LBUTTONDOWN		- left mouse button down
	//	WM_LBUTTONDBLCLK	- left mouse button dobule clicked
	//  WM_LBUTTONUP		- left mouse button came up
	//  WM_RBUTTONDOWN		- right mouse button down
	//	WM_RBUTTONDBLCLK	- right mouse button dobule clicked
	//  WM_RBUTTONUP		- right mouse button came up
	//  WM_MBUTTONDOWN		- middle mouse button down
	//	WM_MBUTTONDBLCLK	- middle mouse button dobule clicked
	//  WM_MBUTTONUP		- middle mouse button came up
	//  WM_XBUTTONDOWN		- browser button down check.  flag to see what one it was
	//	WM_XBUTTONDBLCLK	- browser button dobule clicked.  flag to see what one it was
	//  WM_XBUTTONUP		- browser button came up.  flag to see what one it was
	//  WM_MOUSELEAVE		- special message sent when mousevents are captured
	//  WM_NCACTIVATE		- sent when a user clicked outside the window
	//  WM_MOUSEMOVE		- the mouse has moved this update
	//  WM_DEVICECHANGE		- check for new devices
	//  WM_SYSKEYDOWN		- sent when special keys are pressed
	LRESULT CALLBACK InputProc(HWND _hWnd, UINT _msg, WPARAM _w, LPARAM _l)
	{
		UNREFERENCED_PARAMETER(_hWnd);
		UNREFERENCED_PARAMETER(_l);

		switch (_msg)
		{
#pragma region Keyboard Presses
			case WM_CHAR:
				Events::KeyboardCallback(static_cast<char>(_w) );
			break;
			case WM_KEYDOWN:
				//InternalOutput::GetReference() << "Down: Held Down? " << ((lParam & (1 << 30) ) >> 30) << "\n";
				Events::KeyboardKey(static_cast<uint16_t>(_w), true);
			break;
			case WM_KEYUP:
				Events::KeyboardKey(static_cast<uint16_t>(_w), false);
			break;
#pragma endregion // WM_CHAR is text editors the other messages is for the state of the keyboard
#pragma region Mouse Presses
#pragma region Left Mouse Button
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
				Events::MouseKey(EDInput::MOUSE_LEFT, VK_LBUTTON, true);
				break;
			case WM_LBUTTONUP:
				Events::MouseKey(EDInput::MOUSE_LEFT, VK_LBUTTON, false);
			break;
#pragma endregion // Double click, Down and up messages
#pragma region Right Mouse Button
			case WM_RBUTTONDOWN:
			case WM_RBUTTONDBLCLK:
				Events::MouseKey(EDInput::MOUSE_RIGHT, VK_RBUTTON, true);
			break;
			case WM_RBUTTONUP:
				Events::MouseKey(EDInput::MOUSE_RIGHT, VK_RBUTTON, false);
			break;
#pragma endregion // Double click, Down and up messages
#pragma region Middle Mouse Button
			case WM_MBUTTONDOWN:
			case WM_MBUTTONDBLCLK:
				Events::MouseKey(EDInput::MOUSE_MIDDLE, VK_MBUTTON, true);
				break;
			case WM_MBUTTONUP:
				Events::MouseKey(EDInput::MOUSE_MIDDLE, VK_MBUTTON, false);
				break;
			case WM_MOUSEWHEEL:
				Events::MouseWheel(_w);
				break;
#pragma endregion // Double click, Down, wheel, move, leave window and up messages
#pragma region X1Button & X2Button
			case WM_XBUTTONDBLCLK:
			case WM_XBUTTONDOWN:
				{
					MOUSE_BUTTON button = MAX_BUTTONS;
					uint16_t vk = -1;
					GetBrowserKeys(_w, button, vk);
					Events::MouseKey(button, vk, true);
				}
				break;
			case WM_XBUTTONUP:
				{
					MOUSE_BUTTON button = MAX_BUTTONS;
					uint16_t vk = -1;
					GetBrowserKeys(_w, button, vk);
					Events::MouseKey(button, vk, false);
				}
				break;
#pragma endregion // Double click, Down and up messages for the browser buttons
			case WM_MOUSELEAVE:
				Events::ClearFireEvents(MOUSE_TYPE);
				break;
			//clicked outside the window
			case WM_NCACTIVATE:
				Events::ClearFireEvents(MAX_TYPES);
				break;
			case WM_MOUSEMOVE:
				Events::MouseChange();
				break;
			////clicked on the window
			//case WM_SETFOCUS: // clicked on windowed window
#pragma endregion // All mouse messages
			case WM_DEVICECHANGE:  
				{
					//More types are handled other than gamepads
					//http://msdn.microsoft.com/en-us/library/windows/desktop/aa363480(v=vs.85).aspx
					//It may be useful to make a switch a return based on other needs
					//Right now, this will always return 0 to allow other parts of wndproc to get this message
					GamePad::DeviceChange((uint16_t)_w);
					return 0;
				}
				//JoystickManager::GetReference().RefreshDevices();
				break;
			default:
				return 1;
		}

		//message was handled nothing left to do
		return 0;
	};
}
