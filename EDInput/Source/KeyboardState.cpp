#include "./KeyboardState.h"

namespace EDInput
{
	namespace Keyboard
	{
		KeyboardState::KeyboardState() : TypingCallback(nullptr) 
		{
		}
		void KeyboardState::clear()
		{
			KeyStates.clear();
		}
		void KeyboardState::SetTypingCallback(Typing_Callback _callback)
		{
			TypingCallback = _callback;
		}
		void KeyboardState::SetKeyState(uint16_t _letter, bool _state)
		{
			KeyStates[_letter] = _state;
		}
		void KeyboardState::Callback(char _letter)
		{
			if (nullptr != TypingCallback)
			{
				(*TypingCallback)(_letter);
			}
		}
		bool KeyboardState::IsKeyDown(uint16_t _letter)
		{
			std::map<uint16_t, bool>::iterator itr = KeyStates.find(_letter);
			if (itr != KeyStates.end() )
			{
				return itr->second;
			}

			return false;
		}
	};
};