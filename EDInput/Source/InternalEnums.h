#pragma once

#define INPUT_SET(_key) std::pair<std::string, uint16_t>(#_key, _key)

namespace EDInput
{
	typedef enum : int8_t { MOUSE_LEFT, 
							MOUSE_RIGHT, 
							MOUSE_MIDDLE, 
							MOUSE_X1, 
							MOUSE_X2, 
							MAX_BUTTONS 
							} MOUSE_BUTTON;
};