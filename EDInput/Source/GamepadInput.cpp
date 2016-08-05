#include "./GamepadInput.h"
#include "./EventHelper.h"
#include "./EventHelper.h"

#include <Xinput.h>

#include "../../EDUtilities/InternalOutput.h"

#define MASK_CHECK(m, l, r) ((m & l) != (m & r) )

namespace EDInput
{
	namespace GamePad
	{
		XINPUT_STATE	tempState;

		struct stick
		{
			bool			state;
			float			value[2];
			stick() : state(false)
			{
				ZeroMemory(value, sizeof(float) * 2);
			}
			void clear()
			{
				state = false;
				ZeroMemory(value, sizeof(float) * 2);
			}
			void *GetValue()
			{
				return static_cast<void *>(value);
			}
		};
		struct PadTrigger
		{
			bool state;
			float value;
			PadTrigger() : state(false), value(0.0f) {}
			void clear() 
			{
				state = false;
				value = 0.0f;
			}
			void *GetValue()
			{
				return &value;
			}
		};
		class GameController
		{
		public:
			//left and right sticks
			stick			leftStick;
			stick			rightStick;
			//Left and right triggers
			PadTrigger		leftTrigger;
			PadTrigger		rightTrigger;
			//indicates if any information has changed from one poll to another
			DWORD			lastPacket;
			//Stores all the button states from last frame to limit input messages sent
			WORD			lastState;
			//Is the controller plugged in?
			bool			active;

			GameController() : lastState(0), active(false), lastPacket(0) {}
			void clear()
			{
				active = false;
				lastState = 0;
				lastPacket = 0;
				rightTrigger.clear();
				leftTrigger.clear();
				rightStick.clear();
				leftStick.clear();
			}
		};

		GameController pads[MAX_GAME_PADS];

		void ProcessStickInfo(GAME_PAD_USER _user, float _x, float _y, unsigned int _deadzone, uint16_t _vk, stick * _stick);
		void ProcessTriggerInfo(GAME_PAD_USER _user, BYTE _state, uint16_t _vk, PadTrigger *_pad);
		void ProcessDPad(GAME_PAD_USER _user, WORD _curButtons);
		void ProcessSpecial(GAME_PAD_USER _user, WORD _curButtons);
		void ProcessShoulders(GAME_PAD_USER _user, WORD _curButtons);
		void ProcessButtons(GAME_PAD_USER _user, WORD _curButtons);
		void DetectKeyChange(GAME_PAD_USER _user, uint16_t _flag, WORD _newState, uint16_t _vk);

		void Initialize()
		{
			for (unsigned int i = 0; i < MAX_GAME_PADS; ++i)
			{
				DWORD RetValue = XInputGetState(i, &tempState);
			
				switch (RetValue)
				{
				case ERROR_SUCCESS:
					pads[i].active = true;
					break;
				case ERROR_DEVICE_NOT_CONNECTED:
					pads[i].active = false;
					break;
				default:
					EDUtilities::InternalOutput::GetReference() << "DWORD: " << static_cast<unsigned int>(RetValue) << " Not handled in " << __FILE__ << " " << __LINE__ << "\n";
				};
			}
		}
		void clear(GAME_PAD_USER _user)
		{
			if (_user == MAX_GAME_PADS)
			{
				for (unsigned int i = 0; i < MAX_GAME_PADS; ++i)
				{
					pads[i].clear();
				}
			}
			else
			if (MAX_GAME_PADS > _user)
			{
				pads[_user].clear();
			}
		}
		void InputProc(float _time)
		{
			UNREFERENCED_PARAMETER(_time);

			for (unsigned int i = 0; i < 1; ++i)
			{
				if (false == pads[i].active)
					continue;
	
				if (ERROR_SUCCESS == XInputGetState(i, &tempState) )
				{
					if (tempState.dwPacketNumber != MAX_GAME_PADS)
					{
						GAME_PAD_USER user = static_cast<GAME_PAD_USER>(i);
					
						//Control Sticks
						ProcessStickInfo(user, tempState.Gamepad.sThumbLX, tempState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, VK_PAD_LTHUMB_MOVE, &pads[i].leftStick);
						ProcessStickInfo(user, tempState.Gamepad.sThumbRX, tempState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, VK_PAD_RTHUMB_MOVE, &pads[i].rightStick);

						//Triggers
						ProcessTriggerInfo(user, tempState.Gamepad.bLeftTrigger, VK_PAD_LTRIGGER, &pads[i].leftTrigger);
						ProcessTriggerInfo(user, tempState.Gamepad.bRightTrigger, VK_PAD_RTRIGGER, &pads[i].rightTrigger);

						//D Pad Buttons
						if (MASK_CHECK(0x000F, pads[i].lastState, tempState.Gamepad.wButtons) )
						{
							ProcessDPad(user, tempState.Gamepad.wButtons);
						}

						//Start, back and stick presses
						if (MASK_CHECK(0x00F0,pads[i].lastState, tempState.Gamepad.wButtons) )
						{
							ProcessSpecial(user, tempState.Gamepad.wButtons);
						}

						//Shoulder presses
						if (MASK_CHECK(0x0F00, pads[i].lastState, tempState.Gamepad.wButtons) )
						{
							ProcessShoulders(user, tempState.Gamepad.wButtons);
						}

						if (MASK_CHECK(0xF000, pads[i].lastState, tempState.Gamepad.wButtons) )
						{
							ProcessButtons(user, tempState.Gamepad.wButtons);
						}

						pads[i].lastPacket = tempState.dwPacketNumber;
						pads[i].lastState = tempState.Gamepad.wButtons;
					}
				}
			}
		}
		void DeviceChange(uint16_t _wParam)
		{
			for (unsigned int i = 0; i < MAX_GAME_PADS; ++i)
			{
				DWORD retValue = XInputGetState(i, &tempState);
				switch (retValue)
				{
				case ERROR_SUCCESS:
					if (false == pads[i].active)
					{
						//Get synced up with the packetcount and notify what the current state of the controller is
						pads[i].lastPacket = tempState.dwPacketNumber;
						pads[i].lastState = tempState.Gamepad.wButtons;

						//TODO: alert system.  Input will still work but no one really knows
						uint8_t data[4] = { pads[0].active, pads[1].active, pads[2].active, pads[3].active};
						Events::SendEvent("GamePadChange", data);
					}
					pads[i].active = true;
					break;
				case ERROR_DEVICE_NOT_CONNECTED:
					{
						if (true ==  pads[i].active)
						{
							Events::ClearFireEvents(GAME_PAD_TYPE, static_cast<GAME_PAD_USER>(i) );
						}
						pads[i].active = false;
					}
					pads[i].active = false;
					break;
				default:
					EDUtilities::InternalOutput::GetReference() << "DWORD: " << static_cast<unsigned int>(retValue) << " Not handled in " << __FILE__ << " " << __LINE__ << "\n";
				};
			}
		}
		void BuildCodeMap(std::map<std::string, uint16_t> &_codeMap)
		{
			_codeMap.insert(INPUT_SET(VK_PAD_LTHUMB_MOVE) );
			_codeMap.insert(INPUT_SET(VK_PAD_RTHUMB_MOVE) );
			_codeMap.insert(INPUT_SET(VK_PAD_A) );
			_codeMap.insert(INPUT_SET(VK_PAD_B) );
			_codeMap.insert(INPUT_SET(VK_PAD_X) );
			_codeMap.insert(INPUT_SET(VK_PAD_Y) );
			_codeMap.insert(INPUT_SET(VK_PAD_RSHOULDER) );
			_codeMap.insert(INPUT_SET(VK_PAD_LSHOULDER) );
			_codeMap.insert(INPUT_SET(VK_PAD_DPAD_UP) );
			_codeMap.insert(INPUT_SET(VK_PAD_DPAD_DOWN) );
			_codeMap.insert(INPUT_SET(VK_PAD_DPAD_LEFT) );
			_codeMap.insert(INPUT_SET(VK_PAD_DPAD_RIGHT) );
			_codeMap.insert(INPUT_SET(VK_PAD_START) );
			_codeMap.insert(INPUT_SET(VK_PAD_BACK) );
			_codeMap.insert(INPUT_SET(VK_PAD_LTHUMB_PRESS) );
			_codeMap.insert(INPUT_SET(VK_PAD_RTHUMB_PRESS) );
			_codeMap.insert(INPUT_SET(VK_PAD_LTRIGGER) );
			_codeMap.insert(INPUT_SET(VK_PAD_RTRIGGER) );
		}

		void ProcessStickInfo(GAME_PAD_USER _user, float _x, float _y, unsigned int _deadzone, uint16_t _vk, stick * _stick)
		{
			if (nullptr == _stick)
				return;

			//determine how far the controller is pushed
			float magnitude = sqrt(_x *_x + _y * _y);

			//check if the controller is outside a circular dead zone
			if (magnitude > _deadzone)
			{
				//determine the direction the controller is pushed
				_stick->value[0] = _x / magnitude;
				_stick->value[1] = _y / magnitude;

				//clip the magnitude at its expected maximum value
				if (magnitude > 32767)
				{
					magnitude = 32767;
				}
  
				//adjust magnitude relative to the end of the dead zone
				magnitude -= _deadzone;

				if (false == _stick->state)
				{
					_stick->state = true;
					Events::GameStickChange(_user, _stick->GetValue(), _vk, true);
				}
			}
			//if the controller is in the deadzone zero out the magnitude
			else 
			{
				magnitude = 0.0;
			
				if (true == _stick->state)
				{
					Events::GameStickChange(_user, _stick->GetValue(), _vk, false);
				}
				_stick->clear();
			}
		}
		void ProcessTriggerInfo(GAME_PAD_USER _user, BYTE _state, uint16_t _vk, PadTrigger *_pad)
		{
			if (XINPUT_GAMEPAD_TRIGGER_THRESHOLD < _state)
			{
				_pad->value = static_cast<float>(_state - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / 255.0f;

				if (false == _pad->state)
				{
					_pad->state = true;
					Events::GamePadTrigger(_user, _pad->GetValue(), _vk, _pad->state);
				}
			}
			else
			{
				bool last = _pad->state;
				_pad->clear();
				if (true == last)
				{
					Events::GamePadTrigger(_user, _pad->GetValue(), _vk, _pad->state);
				}
			}
		}
		void ProcessDPad(GAME_PAD_USER _user, WORD _curButtons)
		{
			DetectKeyChange(_user, XINPUT_GAMEPAD_DPAD_UP, _curButtons, VK_PAD_DPAD_UP);
			DetectKeyChange(_user, XINPUT_GAMEPAD_DPAD_DOWN, _curButtons, VK_PAD_DPAD_DOWN);
			DetectKeyChange(_user, XINPUT_GAMEPAD_DPAD_LEFT, _curButtons, VK_PAD_DPAD_LEFT);
			DetectKeyChange(_user, XINPUT_GAMEPAD_DPAD_RIGHT, _curButtons, VK_PAD_DPAD_RIGHT);
		}
		void ProcessSpecial(GAME_PAD_USER _user, WORD _curButtons)
		{
			DetectKeyChange(_user, XINPUT_GAMEPAD_START, _curButtons, VK_PAD_START);
			DetectKeyChange(_user, XINPUT_GAMEPAD_BACK, _curButtons, VK_PAD_BACK);
			DetectKeyChange(_user, XINPUT_GAMEPAD_LEFT_THUMB, _curButtons, VK_PAD_LTHUMB_PRESS);
			DetectKeyChange(_user, XINPUT_GAMEPAD_RIGHT_THUMB, _curButtons, VK_PAD_RTHUMB_PRESS);
		}
		void ProcessShoulders(GAME_PAD_USER _user, WORD _curButtons)
		{
			DetectKeyChange(_user, XINPUT_GAMEPAD_LEFT_SHOULDER, _curButtons, VK_PAD_LSHOULDER);
			DetectKeyChange(_user, XINPUT_GAMEPAD_RIGHT_SHOULDER, _curButtons, VK_PAD_RSHOULDER);
		}
		void ProcessButtons(GAME_PAD_USER _user, WORD _curButtons)
		{
			DetectKeyChange(_user, XINPUT_GAMEPAD_A, _curButtons, VK_PAD_A);
			DetectKeyChange(_user, XINPUT_GAMEPAD_B, _curButtons, VK_PAD_B);
			DetectKeyChange(_user, XINPUT_GAMEPAD_X, _curButtons, VK_PAD_X);
			DetectKeyChange(_user, XINPUT_GAMEPAD_Y, _curButtons, VK_PAD_Y);
		}
		void DetectKeyChange(GAME_PAD_USER _user, uint16_t _flag, WORD _newState, uint16_t _vk)
		{
			uint16_t current = _flag & _newState;
			if ((_flag & pads[_user].lastState) != current)
			{
				Events::GamePadKey(_user, _vk, current > 0 ? true : false);
			}
		}
	};
};