#pragma once

#include "./RegisterInformation.h"

//This file should be used by applications
//Purpose: used by controllers to register for events
namespace EDInput
{
	//Purpose: called before AddInputCallack to send in the result to _buttonPress
	//Return: Convert the string into an enum to help register callback functions
	ButtonState GetKeyStateByName(std::string _name);

	//Purpose: called before AddInputCallack to send in the result to _VKey
	//Return: Convert the string the trigger type stored in input
	uint16_t GetKeyByName(std::string _name);

	//Purpose: called before AddInputCallack to send in the result to _user
	//Only used for gamepads
	//return: Convert the string to a GAME_PAD_USER enum
	GAME_PAD_USER GetUserByName(std::string _name);

	/// Controls input within the engine, allowing for registration for events
	// vKey : virtual key code
    // ButtonState : from input enums
	// eventName : Event to send when this trigger is activated
	// void *: (optional) if this should be sent to any listener
	// uint8_t: (optional) Input mode(s) the event should be sent default is BASIC
	// GAME_PAD_USER: (optional) what controller should this message be assigned to.  Keyboard and mouse are the default value of max
	unsigned int AddInputCallback(	InputTypes _input, 
									const uint16_t _VKey, 
									ButtonState _buttonPress, 
									const std::string &_event, 
									Event_Callback _callback, 
									void * _listener = nullptr, 
									uint8_t _mode = BASIC_INPUT,
									GAME_PAD_USER _user = MAX_GAME_PADS);
};