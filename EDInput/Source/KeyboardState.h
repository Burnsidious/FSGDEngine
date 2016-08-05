#pragma once

#include "../RegisterInformation.h"

namespace EDInput
{
	namespace Keyboard
	{
		class KeyboardState
		{
		public:
			KeyboardState();
			void clear();

			void Callback(char _letter);

			void SetTypingCallback(Typing_Callback _callback);
			void SetKeyState(uint16_t _letter, bool _state);

			bool IsKeyDown(uint16_t _letter);
		private:
			Typing_Callback				TypingCallback;
			std::map<uint16_t, bool>	KeyStates;
		};
	};
	
};