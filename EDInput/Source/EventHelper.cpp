#include "./EventHelper.h"

#include "./MouseState.h"
#include "./KeyboardState.h"
#include "./GamepadInput.h"

#include "./InputMessageHandler.h"

#include "../../EDEventManager/EventManager.h"
#include "../../EDUtilities/InternalOutput.h"

namespace EDInput
{
	// Window handle for obtaining cursor position
	HWND hWnd = nullptr;
	//Mode to limit messages sent
	InputMode inputMode = BASIC_INPUT;

	//Mouse Information
	Mouse::MouseState mouseState;

	//Keyboard info
	Keyboard::KeyboardState keyboardState;

	void BuildKeyCodeMapping();
	POINT GetWindowsMouse();

	namespace Events
	{
		// Used only internally within the input manager (key to access events to trigger based on input)
		struct Trigger
		{
			uint16_t		button;				
			ButtonState		buttonState;		
			InputTypes		type;
			GAME_PAD_USER	user;//TODO may want to put this information in Joystick controller and remove teh need for duplicate trigger types based on controller number

			Trigger(uint16_t _button, ButtonState _buttonState, InputTypes _type, GAME_PAD_USER _num = MAX_GAME_PADS) : button(_button), buttonState(_buttonState), type(_type), user(_num){ }

			bool operator < (const Trigger &right) const
			{
				if (button == right.button)
				{
					if (buttonState == right.buttonState)
					{
						if (type == right.type)
						{
							return user < right.user;
						}
						return type < right.type;
					}

					return buttonState < right.buttonState;
				}
				return button < right.button;
			}
		};

		//Information about persistant messages to reclaim them in case of a lost focus
		struct FireIdentifier
		{
			int index;
			InputTypes type;
			GAME_PAD_USER user;
			FireIdentifier(int _index, InputTypes _type, GAME_PAD_USER _user) : index(_index), type(_type), user(_user) {}
			FireIdentifier(const FireIdentifier &_in) : index(_in.index), type(_in.type), user(_in.user) {}
			FireIdentifier() : index(-1), type(MAX_TYPES), user(MAX_GAME_PADS) {}
		};

		//stored by InputTriggers to use the event system
		struct EventMessage
		{
			std::string eventName;
			uint16_t mode;

			EventMessage(const std::string &_eventName, uint16_t _mode) : eventName(_eventName), mode(_mode) { }

			inline bool IsValidMode(InputMode _mode) const
			{
				return (mode & _mode) == _mode;
			}
		};

		//commonly used iterator
		typedef std::list<EventMessage>::iterator EventItr;

		std::map<Trigger, std::list<EventMessage> >		inputTriggers;
		std::map<std::string, FireIdentifier>			firingEvents;

		//Internal functions
		void BroadcastTrigger(Trigger &_trigger, const void *_data = nullptr);
		bool BroadcastFireTrigger(Trigger &_trigger, const void *_data = nullptr);
		void StopTrigger(Trigger &_trigger, bool _clear = true);
#include <Xinput.h>
		void Initialize(HWND _hWnd) 
		{
			InputMode mode = BASIC_INPUT;
			MessageHandler::SetInputMode(-1, &mode, nullptr);
			BuildKeyCodeMapping();
			hWnd = _hWnd;

			mouseState.Initialize(_hWnd);
			mouseState.SetMouseFocus(true);
		
			EDEventManager::EventManager *eventManager = EDEventManager::EventManager::GetInstance();

			eventManager->RegisterForEvent("Toggle Input", nullptr, &MessageHandler::ToggleInputCallBack);
			eventManager->RegisterForEvent("Set Cursor", nullptr, &MessageHandler::SetCursorEvent);
			eventManager->RegisterForEvent("Show Cursor", nullptr, &MessageHandler::ShowCursorEvent);
			eventManager->RegisterForEvent("Set Input", nullptr, &MessageHandler::SetInputMode);
		}
		void ClearFireEvents(InputTypes _type, GAME_PAD_USER _user)
		{
			std::vector<std::string> temp;

			for (auto itr: firingEvents)
			{
				Trigger trigger(itr.second.index, INPUT_RELEASED,  itr.second.type, itr.second.user);

				if (GAME_PAD_TYPE ==  _type)
				{
					if (_user & itr.second.user)
					{
						StopTrigger(trigger, false);
						temp.push_back(itr.first);
					}
				}
				else
				if (itr.second.type & _type)
				{
					StopTrigger(trigger, false);
					temp.push_back(itr.first);
				}
			}
			for (auto itr: temp)
			{
				firingEvents.erase(itr);
			}
			temp.clear();

			if (KEYBOARD_TYPE & _type)
			{
				keyboardState.clear();
			}
			if (MOUSE_TYPE & _type)
			{
				mouseState.SetMouseFocus(false);
			}
			if ((GAME_PAD_TYPE == _type) )
			{
				if (MAX_GAME_PADS == _user)
				{
					GamePad::clear();
				}
				else
				{
					GamePad::clear(_user);
				}	
			}
			if (MAX_TYPES == _type)
			{
				firingEvents.clear();
			}
		}

		void MouseKey(MOUSE_BUTTON _button, uint16_t _vk, bool _down)
		{
			if (true == _down)
			{
				//// If the button is already down, ignore KEY_PRESSED and KEY_HELD triggers.
				if (INPUT_HELD == mouseState.IsPressed(_button) )
					return;

				// Data we need: button, event type.
				Trigger trigger(_vk, INPUT_PRESSED, MOUSE_TYPE);
				BroadcastTrigger(trigger, mouseState.GetMousePosition() );

				trigger.buttonState = INPUT_HELD;

				if (BroadcastFireTrigger(trigger, mouseState.GetMousePosition() ) )
				{
					mouseState.SetState(_button, INPUT_HELD);
				}
			}
			else
			{
				Trigger trigger(_vk, INPUT_RELEASED, MOUSE_TYPE);

				if (INPUT_RELEASED == mouseState.IsPressed(_button) )
				{
					return;
				}

				// Mark this button as being up.
				mouseState.SetState(_button, INPUT_RELEASED);

				StopTrigger(trigger);
			}
		}
		void MouseChange()
		{
			POINT mouse = GetWindowsMouse();

			Trigger  trigger = Trigger(-1, MAX_STATES, MOUSE_TYPE);

			if (false == mouseState.IsMouseInFocus() )
			{
				mouseState.SetMouseFocus(true);
			}

			BroadcastTrigger(trigger, mouseState.GetMousePosition() );
		}
		void MouseWheel(WPARAM _w)
		{
			MouseWheelInfo temp;
			temp.Change = GET_WHEEL_DELTA_WPARAM(_w) > 1 ? 1 : -1;
			temp.MousePosition = *mouseState.GetMousePosition();
			temp.WheelPosition = mouseState.ChangeWheelData(temp.Change);

			//Might need a new enum for moved
			Trigger trigger(VK_MBUTTON, MAX_STATES, MOUSE_TYPE);

			BroadcastTrigger(trigger, static_cast<void *>(&temp) );
		}

		void KeyboardCallback(char _letter)
		{
			if (TYPED_INPUT & inputMode)
			{
				keyboardState.Callback(_letter);
			}
		}
		void KeyboardKey(uint16_t _letter, bool _down)
		{
			if (true == _down)
			{
				if (keyboardState.IsKeyDown(_letter) )
					return;

				Trigger trigger(_letter, INPUT_PRESSED, KEYBOARD_TYPE);

				BroadcastTrigger(trigger, nullptr);
		
				trigger.buttonState = INPUT_HELD;
				if (BroadcastFireTrigger(trigger) )
				{
					keyboardState.SetKeyState(_letter, true);
				}
			}
			else
			{
				Trigger trigger(_letter, INPUT_RELEASED, KEYBOARD_TYPE);
				if (!keyboardState.IsKeyDown(_letter) )
					return;

				StopTrigger(trigger);

				keyboardState.SetKeyState(_letter, false);
			}
		}

		void GamePadKey(GAME_PAD_USER _user, uint16_t _vk, bool _down)
		{
			if (true == _down)
			{

				Trigger trigger(_vk, INPUT_PRESSED, GAME_PAD_TYPE, _user);
				BroadcastTrigger(trigger, nullptr);

				trigger.buttonState = INPUT_HELD;
				BroadcastFireTrigger(trigger);
			}
			else
			{
				Trigger trigger(_vk, INPUT_RELEASED, GAME_PAD_TYPE, _user);

				StopTrigger(trigger);
			}
		}
		void GamePadTrigger(GAME_PAD_USER _user, void *_value, uint16_t _vk, bool _down)
		{
			if (true == _down)
			{
				Trigger trigger(_vk, INPUT_PRESSED, GAME_PAD_TYPE, _user);
				BroadcastTrigger(trigger, _value);

				trigger.buttonState = INPUT_HELD;
				BroadcastFireTrigger(trigger, _value);
			}
			else
			{
				Trigger trigger(_vk, INPUT_RELEASED, GAME_PAD_TYPE, _user);
				StopTrigger(trigger);
			}
		}
		void GameStickChange(GAME_PAD_USER _user, void *_data, uint16_t _vk, bool _down)
		{
			if(true == _down)
			{
				Trigger trigger(_vk, INPUT_PRESSED, GAME_PAD_TYPE,_user);
				BroadcastTrigger(trigger, _data);

				trigger.buttonState = INPUT_HELD;
				BroadcastFireTrigger(trigger, _data);
			}
			else
			{
				Trigger trigger(_vk, INPUT_RELEASED, GAME_PAD_TYPE,_user);
				StopTrigger(trigger, true);
			}
		}

		void AddTrigger(InputTypes _input, const uint16_t _vk, ButtonState _state, const std::string &_name, uint8_t _inputMode, GAME_PAD_USER _user)
		{
			Trigger trigger(_vk, _state, _input, _user);
			inputTriggers[trigger].push_back(EventMessage(_name, _inputMode) );

			 //If this is a key down event add an event to remove it when the key is up
			if(_state == INPUT_HELD)
			{
				trigger.buttonState = INPUT_RELEASED;
				inputTriggers[trigger].push_back(EventMessage(_name, _inputMode) );
			}
		}
		unsigned int RegisterEvent(const std::string &_event, void *_listener, Event_Callback _callback)
		{
			return EDEventManager::EventManager::GetInstance()->RegisterForEvent(_event, _listener, _callback);
		}
		void SendEvent(const std::string &_event, void *_data)
		{
			EDEventManager::EventManager *eventManager = EDEventManager::EventManager::GetInstance();
			eventManager->PostEvent(_event, _data, EDEventManager::EM_PRIORITY_ABOVE_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);
		}

		//Helper functions used for commonly used lines of code
		void BroadcastTrigger(Trigger &_trigger, const void *_data)
		{
			EDEventManager::EventManager *eventManager = EDEventManager::EventManager::GetInstance();

			if (inputTriggers.find(_trigger) == inputTriggers.end() )
				return;

			for (EventItr message = inputTriggers[_trigger].begin(); message != inputTriggers[_trigger].end(); ++message)
			{
				// Is this the right input mode for this event? If not, skip this message.
				if (!message->IsValidMode(inputMode))
					continue;

				eventManager->PostEvent(message->eventName, _data, EDEventManager::EM_PRIORITY_ABOVE_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);
			}
		}
		bool BroadcastFireTrigger(Trigger &_trigger, const void *_data)
		{
			EDEventManager::EventManager *eventManager = EDEventManager::EventManager::GetInstance();

			if (inputTriggers.find(_trigger) == inputTriggers.end() )
				return false;

			for (EventItr message = inputTriggers[_trigger].begin(); message != inputTriggers[_trigger].end(); ++message)
			{
				if (!message->IsValidMode(inputMode))
					continue;

				eventManager->PostEvent(message->eventName, _data, EDEventManager::EM_PRIORITY_ABOVE_NORMAL, EDEventManager::EM_FLAG_PERSISTENT);

				firingEvents[message->eventName] = FireIdentifier(_trigger.button, _trigger.type, _trigger.user);
			}

			return true;
		}
		void StopTrigger(Trigger &_trigger, bool _clear)
		{
			EDEventManager::EventManager *eventManager = EDEventManager::EventManager::GetInstance();

			if (inputTriggers.find(_trigger) == inputTriggers.end() )
				return;

			std::vector<std::string> temp;

			for (EventItr message = inputTriggers[_trigger].begin(); message != inputTriggers[_trigger].end(); ++message)
			{
				if (firingEvents.find(std::string(message->eventName) ) != firingEvents.end() )
				{
					eventManager->DeletePersistentEvent(message->eventName);
					temp.push_back(message->eventName);
				}

				if (message->IsValidMode(inputMode))
				{
					eventManager->PostEvent(message->eventName, nullptr, EDEventManager::EM_PRIORITY_ABOVE_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);
				}
			}

			if (_clear)
			{
				for (auto itr: temp)
				{
					firingEvents.erase(itr);
				}
			}
		}
	};
	
	void SetTypingCallback(Typing_Callback _typingCallback)
	{
		keyboardState.SetTypingCallback(_typingCallback);
	}
	POINT GetWindowsMouse()
	{
		POINT mouse;

		GetCursorPos(&mouse);
		ScreenToClient(hWnd, &mouse);
		mouseState.SetMousePosition(&mouse);

		return mouse;
	}
}