#pragma once

#include "./RegisterInformation.h"

//This file should be used by the base system only
namespace EDInput
{
	//Called from windmain for set-up
	void Initialize(HWND _hWnd);

	//TODO might want to make this a message and remove the dependancy on registerinformation.h
	void SetTypingCallback(Typing_Callback _typingCallback);

	//  PURPOSE:  Processes messages for gamepad input.
	// Input: is just a placeholder right now
	void Update(float _time);

	//Called from windproc
	//return:	0 If the message was handled - as in there is nothing left to do
	//			1 if the message was not handled - as in there is at least 1 thing to do
	LRESULT CALLBACK InputProc(HWND, UINT, WPARAM, LPARAM);
};