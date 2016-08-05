#include "../InputManager.h"
#include "./EventHelper.h"
#include "./GamepadInput.h"

#include "../EDUtilities/InternalOutput.h"

namespace EDInput
{
	//Name matched up with VK code
	std::map<std::string, uint16_t>	keyCodeMapping;

	ButtonState GetKeyStateByName(std::string _name)
	{
		ButtonState returnState = MAX_STATES;

		if("Released" == _name)
		{
			returnState = INPUT_RELEASED;
		}
		else 
		if("Pressed" == _name)
		{
			returnState = INPUT_PRESSED;
		}
		else 
		if("Held" == _name)
		{
			returnState = INPUT_HELD;
		}
		else
		{
			EDUtilities::InternalOutput::GetReference() << "Could not find state " << _name << " in EDInput::GetKeyStateByName\n"; 
		}

		return returnState;
	}
	uint16_t GetKeyByName(std::string _name) 
	{ 
		return keyCodeMapping[_name]; 
	}
	GAME_PAD_USER GetUserByName(std::string _name)
	{
		GAME_PAD_USER returnValue = MAX_GAME_PADS;
		if ("One" == _name)
		{
			 returnValue = PAD_0;
		}
		else
		if ("Two" == _name)
		{
			returnValue = PAD_1;
		}
		else
		if ("Three" == _name)
		{
			returnValue = PAD_2;
		}
		else
		if ("Four" == _name)
		{
			returnValue = PAD_3;
		}
		return returnValue;
	}
	unsigned int AddInputCallback(InputTypes _input, const uint16_t _VKey, ButtonState _buttonPress, const std::string &_event, Event_Callback _callback, 
									void * _listener, uint8_t _mode, GAME_PAD_USER _user)
	{
		switch (_input)
		{
		case KEYBOARD_TYPE:
		case MOUSE_TYPE:
		case GAME_PAD_TYPE:
			Events::AddTrigger(_input, _VKey, _buttonPress, _event, _mode, _user);
			break;
		default:
			EDUtilities::InternalOutput::GetReference() << "ERROR: Input Type " << _input << " not supported for message " << _event << "\n";
			return -1;
		};

		return Events::RegisterEvent(_event, _listener, _callback);
	}
	void BuildKeyCodeMapping()
	{
		// Map the letter keys to the letter strings
		std::string value("A");
		while(value[0] <= 'Z')
		{
			keyCodeMapping.insert(std::pair<std::string, uint16_t>(value, value[0]));
			++value[0];
		}

		// Map the virtual key codes to the string of the same text
		keyCodeMapping.insert(INPUT_SET(VK_SPACE) );
		keyCodeMapping.insert(INPUT_SET(VK_LBUTTON) );
		keyCodeMapping.insert(INPUT_SET(VK_RBUTTON) );
		keyCodeMapping.insert(INPUT_SET(VK_CANCEL) );
		keyCodeMapping.insert(INPUT_SET(VK_MBUTTON) );

#if(_WIN32_WINNT >= 0x0500)
		keyCodeMapping.insert(INPUT_SET(VK_XBUTTON1) );
		keyCodeMapping.insert(INPUT_SET(VK_XBUTTON2) );
#endif
		keyCodeMapping.insert(INPUT_SET(VK_BACK) );
		keyCodeMapping.insert(INPUT_SET(VK_TAB) );
		keyCodeMapping.insert(INPUT_SET(VK_CLEAR) );
		keyCodeMapping.insert(INPUT_SET(VK_RETURN) );
		keyCodeMapping.insert(INPUT_SET(VK_SHIFT) );
		keyCodeMapping.insert(INPUT_SET(VK_CONTROL) );
		keyCodeMapping.insert(INPUT_SET(VK_MENU) );
		keyCodeMapping.insert(INPUT_SET(VK_PAUSE) );
		keyCodeMapping.insert(INPUT_SET(VK_CAPITAL) );
		keyCodeMapping.insert(INPUT_SET(VK_KANA) );
		keyCodeMapping.insert(INPUT_SET(VK_HANGEUL) );
		keyCodeMapping.insert(INPUT_SET(VK_HANGUL) );
		keyCodeMapping.insert(INPUT_SET(VK_JUNJA) );
		keyCodeMapping.insert(INPUT_SET(VK_FINAL) );
		keyCodeMapping.insert(INPUT_SET(VK_HANJA) );
		keyCodeMapping.insert(INPUT_SET(VK_KANJI) );
		keyCodeMapping.insert(INPUT_SET(VK_ESCAPE) );
		keyCodeMapping.insert(INPUT_SET(VK_CONVERT) );
		keyCodeMapping.insert(INPUT_SET(VK_NONCONVERT) );
		keyCodeMapping.insert(INPUT_SET(VK_ACCEPT) );
		keyCodeMapping.insert(INPUT_SET(VK_MODECHANGE) );
		keyCodeMapping.insert(INPUT_SET(VK_SPACE) );
		keyCodeMapping.insert(INPUT_SET(VK_PRIOR) );
		keyCodeMapping.insert(INPUT_SET(VK_NEXT) );
		keyCodeMapping.insert(INPUT_SET(VK_END ) );
		keyCodeMapping.insert(INPUT_SET(VK_HOME) );
		keyCodeMapping.insert(INPUT_SET(VK_LEFT) );
		keyCodeMapping.insert(INPUT_SET(VK_UP) );
		keyCodeMapping.insert(INPUT_SET(VK_RIGHT) );
		keyCodeMapping.insert(INPUT_SET(VK_DOWN) );
		keyCodeMapping.insert(INPUT_SET(VK_SELECT) );
		keyCodeMapping.insert(INPUT_SET(VK_PRINT) );
		keyCodeMapping.insert(INPUT_SET(VK_EXECUTE) );
		keyCodeMapping.insert(INPUT_SET(VK_SNAPSHOT) );
		keyCodeMapping.insert(INPUT_SET(VK_INSERT) );
		keyCodeMapping.insert(INPUT_SET(VK_DELETE) );
		keyCodeMapping.insert(INPUT_SET(VK_HELP) );
		keyCodeMapping.insert(INPUT_SET(VK_LWIN) );
		keyCodeMapping.insert(INPUT_SET(VK_RWIN) );
		keyCodeMapping.insert(INPUT_SET(VK_APPS) );
		keyCodeMapping.insert(INPUT_SET(VK_SLEEP) );
		keyCodeMapping.insert(INPUT_SET(VK_NUMPAD0)	);
		keyCodeMapping.insert(INPUT_SET(VK_NUMPAD1)	);
		keyCodeMapping.insert(INPUT_SET(VK_NUMPAD2)	);
		keyCodeMapping.insert(INPUT_SET(VK_NUMPAD3)	);
		keyCodeMapping.insert(INPUT_SET(VK_NUMPAD4)	);
		keyCodeMapping.insert(INPUT_SET(VK_NUMPAD5)	);
		keyCodeMapping.insert(INPUT_SET(VK_NUMPAD6)	);
		keyCodeMapping.insert(INPUT_SET(VK_NUMPAD7)	);
		keyCodeMapping.insert(INPUT_SET(VK_NUMPAD8)	);
		keyCodeMapping.insert(INPUT_SET(VK_NUMPAD9)	);
		keyCodeMapping.insert(INPUT_SET(VK_MULTIPLY) );
		keyCodeMapping.insert(INPUT_SET(VK_ADD) );
		keyCodeMapping.insert(INPUT_SET(VK_SEPARATOR) );
		keyCodeMapping.insert(INPUT_SET(VK_SUBTRACT) );
		keyCodeMapping.insert(INPUT_SET(VK_DECIMAL) );
		keyCodeMapping.insert(INPUT_SET(VK_DIVIDE) );
		keyCodeMapping.insert(INPUT_SET(VK_F1) );
		keyCodeMapping.insert(INPUT_SET(VK_F2) );
		keyCodeMapping.insert(INPUT_SET(VK_F3) );
		keyCodeMapping.insert(INPUT_SET(VK_F4) );
		keyCodeMapping.insert(INPUT_SET(VK_F5) );
		keyCodeMapping.insert(INPUT_SET(VK_F6) );
		keyCodeMapping.insert(INPUT_SET(VK_F7) );
		keyCodeMapping.insert(INPUT_SET(VK_F8) );
		keyCodeMapping.insert(INPUT_SET(VK_F9) );
		keyCodeMapping.insert(INPUT_SET(VK_F10)	);
		keyCodeMapping.insert(INPUT_SET(VK_F11)	);
		keyCodeMapping.insert(INPUT_SET(VK_F12)	);
		keyCodeMapping.insert(INPUT_SET(VK_F13)	);
		keyCodeMapping.insert(INPUT_SET(VK_F14)	);
		keyCodeMapping.insert(INPUT_SET(VK_F15)	);
		keyCodeMapping.insert(INPUT_SET(VK_F16)	);
		keyCodeMapping.insert(INPUT_SET(VK_F17)	);
		keyCodeMapping.insert(INPUT_SET(VK_F18)	);
		keyCodeMapping.insert(INPUT_SET(VK_F19)	);
		keyCodeMapping.insert(INPUT_SET(VK_F20)	);
		keyCodeMapping.insert(INPUT_SET(VK_F21)	);
		keyCodeMapping.insert(INPUT_SET(VK_F22)	);
		keyCodeMapping.insert(INPUT_SET(VK_F23)	);
		keyCodeMapping.insert(INPUT_SET(VK_F24)	);
		keyCodeMapping.insert(INPUT_SET(VK_NUMLOCK) );
		keyCodeMapping.insert(INPUT_SET(VK_SCROLL) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_NEC_EQUAL) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_FJ_JISHO) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_FJ_MASSHOU) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_FJ_TOUROKU) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_FJ_LOYA) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_FJ_ROYA) );
		keyCodeMapping.insert(INPUT_SET(VK_LSHIFT) );
		keyCodeMapping.insert(INPUT_SET(VK_RSHIFT) );
		keyCodeMapping.insert(INPUT_SET(VK_LCONTROL) );
		keyCodeMapping.insert(INPUT_SET(VK_RCONTROL) );
		keyCodeMapping.insert(INPUT_SET(VK_LMENU) );
		keyCodeMapping.insert(INPUT_SET(VK_RMENU) );

#if(_WIN32_WINNT >= 0x0500)
		keyCodeMapping.insert(INPUT_SET(VK_BROWSER_BACK) );
		keyCodeMapping.insert(INPUT_SET(VK_BROWSER_FORWARD) );
		keyCodeMapping.insert(INPUT_SET(VK_BROWSER_REFRESH) );
		keyCodeMapping.insert(INPUT_SET(VK_BROWSER_STOP) );
		keyCodeMapping.insert(INPUT_SET(VK_BROWSER_SEARCH) );
		keyCodeMapping.insert(INPUT_SET(VK_BROWSER_FAVORITES) );
		keyCodeMapping.insert(INPUT_SET(VK_BROWSER_HOME) );
		keyCodeMapping.insert(INPUT_SET(VK_VOLUME_MUTE) );
		keyCodeMapping.insert(INPUT_SET(VK_VOLUME_DOWN) );
		keyCodeMapping.insert(INPUT_SET(VK_VOLUME_UP) );
		keyCodeMapping.insert(INPUT_SET(VK_MEDIA_NEXT_TRACK) );
		keyCodeMapping.insert(INPUT_SET(VK_MEDIA_PREV_TRACK) );
		keyCodeMapping.insert(INPUT_SET(VK_MEDIA_STOP) );
		keyCodeMapping.insert(INPUT_SET(VK_MEDIA_PLAY_PAUSE) );
		keyCodeMapping.insert(INPUT_SET(VK_LAUNCH_MAIL) );
		keyCodeMapping.insert(INPUT_SET(VK_LAUNCH_MEDIA_SELECT) );
		keyCodeMapping.insert(INPUT_SET(VK_LAUNCH_APP1) );
		keyCodeMapping.insert(INPUT_SET(VK_LAUNCH_APP2) );
#endif
		keyCodeMapping.insert(INPUT_SET(VK_OEM_1) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_PLUS) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_COMMA) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_MINUS) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_PERIOD) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_2) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_3) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_4) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_5) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_6) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_7) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_8) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_AX) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_102) );
		keyCodeMapping.insert(INPUT_SET(VK_ICO_HELP) );
		keyCodeMapping.insert(INPUT_SET(VK_ICO_00) );

#if(WINVER >= 0x0400)
		keyCodeMapping.insert(INPUT_SET(VK_PROCESSKEY) );
#endif
		keyCodeMapping.insert(INPUT_SET(VK_ICO_CLEAR) );

#if(_WIN32_WINNT >= 0x0500)
		keyCodeMapping.insert(INPUT_SET(VK_PACKET) );
#endif
		keyCodeMapping.insert(INPUT_SET(VK_OEM_RESET) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_JUMP) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_PA1) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_PA2) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_PA3) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_WSCTRL) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_CUSEL) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_ATTN) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_FINISH) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_COPY) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_AUTO) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_ENLW) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_BACKTAB) );
		keyCodeMapping.insert(INPUT_SET(VK_ATTN) );
		keyCodeMapping.insert(INPUT_SET(VK_CRSEL) );
		keyCodeMapping.insert(INPUT_SET(VK_EXSEL) );
		keyCodeMapping.insert(INPUT_SET(VK_EREOF) );
		keyCodeMapping.insert(INPUT_SET(VK_PLAY) );
		keyCodeMapping.insert(INPUT_SET(VK_ZOOM) );
		keyCodeMapping.insert(INPUT_SET(VK_NONAME) );
		keyCodeMapping.insert(INPUT_SET(VK_PA1) );
		keyCodeMapping.insert(INPUT_SET(VK_OEM_CLEAR) );

		keyCodeMapping.insert(INPUT_SET(VK_MOUSE_MOVE) );

		GamePad::BuildCodeMap(keyCodeMapping);
	}
}