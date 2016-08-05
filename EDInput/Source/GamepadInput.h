#pragma once

#include "../RegisterInformation.h"

//This is the same as windowsInput but this is the only file that has access to XInput
namespace EDInput
{
	namespace GamePad
	{
		void Initialize();
		void clear(GAME_PAD_USER _user = MAX_GAME_PADS);
		void InputProc(float _time);
		void DeviceChange(uint16_t _wParam);
		void BuildCodeMap(std::map<std::string, uint16_t> &_codeMap);
		
	};
};