#pragma once

#include "./InternalEnums.h"
#include "../RegisterInformation.h"
//This is a helper file to send messages back to the engine
//THis is the only access to event maanger
namespace EDInput
{
	namespace Events
	{
		void Initialize(HWND _hWnd) ;
		void ClearFireEvents(InputTypes _type, GAME_PAD_USER _user = MAX_GAME_PADS);
		
		void MouseKey(MOUSE_BUTTON _button, uint16_t _vk, bool _down);
		void MouseChange();
		void MouseWheel(WPARAM _w);

		void KeyboardCallback(char _letter);
		void KeyboardKey(uint16_t _vk, bool _down);

		void GamePadKey(GAME_PAD_USER _user, uint16_t _vk, bool _down);
		void GamePadTrigger(GAME_PAD_USER _user, void *_value, uint16_t _vk, bool _down);
		void GameStickChange(GAME_PAD_USER _user, void *_data, uint16_t _vk, bool _down);
	
		void AddTrigger(InputTypes _input, const uint16_t _vk, ButtonState _state, const std::string &_name, uint8_t _inputMode, GAME_PAD_USER _user);
		unsigned int RegisterEvent(const std::string &_event, void *_listener, Event_Callback _callback);

		void SendEvent(const std::string &_event, void *_data);
	};
};