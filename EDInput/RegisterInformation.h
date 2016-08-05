#pragma once

//Messages to reduce unwanted messages
//THis may cause collisions with expanded functionality of XINPUT buttons
//These are created to get one stick move message for all the different types of messagees XINPUT offer
//Data sent is a float[2].  [0] is the x and [1] is the y data sent
#define VK_PAD_LTHUMB_MOVE  0x58F0
#define VK_PAD_RTHUMB_MOVE  0x58F1
//Mouse message is sent with this key to keep it consistant with the rest of messages stored
//On a mouse move the button state is MAX_STATES
//Data sent on this message is POINT *
#define VK_MOUSE_MOVE		0xF0F0

//Message info for: VK_PAD_LTRIGGER and VK_PAD_RTRIGGER
//Data sent is a float * for how much each trigger is getting held down between 0.0 and 1.0

//Event "GamePadChange" is sent when a controller is detected or a connected controller was removed
//Data is uint8_t[4] - the result is a bool. e.g. unit8_t[0] == PAD_0, unit8_t[1] == PAD_1 
namespace EDInput
{
	//Event system callback
	//TODO might need a single file so if there is a change to the message system this will not have to change as well
	typedef void (*Event_Callback)(unsigned int, const void *, void *);

	//Callback function to reeice input messages when typing is needed
	typedef void (*Typing_Callback)(char);

	//Possible input modes
	//Set new types using bits
	typedef enum : uint8_t {	BASIC_INPUT = 1 << 0, 
								TYPED_INPUT = 1 << 1, 
								MODE_ANY = 0xFF 
								} InputMode;

	//What type of input are we looking for
	typedef enum : int8_t { KEYBOARD_TYPE, 
							MOUSE_TYPE, 
							GAME_PAD_TYPE, 
							MAX_TYPES 
							} InputTypes;

	//When messages should be sent
	typedef enum : int8_t { INPUT_RELEASED, 
							INPUT_PRESSED, 
							INPUT_HELD, 
							MAX_STATES 
							} ButtonState;	

	//What device should this message look for
	//Mouse and Keyboard are MAX_GAME_PADS
	typedef enum : uint8_t {	PAD_0,
								PAD_1, 
								PAD_2,
								PAD_3,
								//May need an ALL_PADS enum if we need to register for a key of any pad
								MAX_GAME_PADS
							} GAME_PAD_USER;

	//Message data when mousewheel messages are sent
	//These messages can never be held middle mouse clicked is another message
	//The button state for these massages is MAX_STATES
	struct MouseWheelInfo
	{
		//Also accounts for where the mouse is
		POINT	MousePosition;
		//Total count of Wheel position
		int16_t WheelPosition;
		//1 or -1
		int16_t Change;
	};
};