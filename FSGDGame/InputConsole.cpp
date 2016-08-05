#include "InputConsole.h"
#include "WorldData.h"

#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/ContentManager.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDEventManager/EventManager.h"
#include "../EDMemoryManager/MemoryManager.h"
#include "../EDInput/RegisterInformation.h"

using namespace DirectX;
using namespace EDRendererD3D;
using namespace EDUtilities;
using std::string;
using std::wstring;
using std::set;

InputConsole *InputConsole::instancePtr = 0;

InputConsole::InputConsole(void) 
{ 
    inputRenderString.text = L"";
	inputRenderString.color = XMFLOAT4(0, 0, 0, 1);
	completionRenderString.text = L"";
	completionRenderString.color = XMFLOAT4(0, 0, 1, 1);
	isEnabled = false; 
	lastRememberedIndex = 0;

	xRenderString = 2;
	yRenderString = Renderer::GetResolutionHeight() / 32 * 31;

	for(int index = 0; index < NumStringsRemembered; ++index)
	{
		rememberedStrings[index] = L"";
	}
}

InputConsole &InputConsole::GetReference()
{
	if(0 == instancePtr)
	{
		instancePtr = new InputConsole;
		instancePtr->Initialize();
	}
	return *instancePtr;
}

void InputConsole::DeleteInstance()
{
	delete instancePtr;
	instancePtr = 0;
}

InputConsole::~InputConsole(void)
{

}

void InputConsole::ToggleConsoleCallback(unsigned int eventId, const void *eventData, void *listener)
{
	GetReference().isEnabled = !GetReference().isEnabled;

	EDEventManager::EventManager *eventManager = EDEventManager::EventManager::GetInstance();
	eventManager->PostEvent("Toggle Input");


	//uint8_t mode = GetReference().isEnabled ? EDInput::TYPED_INPUT : EDInput::BASIC_INPUT;
	//eventManager->PostEvent("Set Input", &mode);
}

void InputConsole::CompleteConsoleStringCallback(unsigned int eventId, const void *eventData, void *listener)
{
	InputConsole &console = GetReference();
	if(console.isEnabled)
	{
		console.inputRenderString.text = console.completionRenderString.text;
	}
	console.UpdateString(console.inputRenderString, console.inputRenderString.text);
}

void InputConsole::RememberConsoleStringCallback(unsigned int eventId, const void *eventData, void *listener)
{
	InputConsole &console = GetReference();

	// Find which string we are currently on
	int foundIndex = -1;

	for(int index = 0; index < NumStringsRemembered; ++index)
	{
		if(console.inputRenderString.text == console.rememberedStrings[index])
		{
			foundIndex = index;
			break;
		}
	}

	// test if we did not find the string
	if(-1 == foundIndex)
	{
		foundIndex = console.lastRememberedIndex;
	}
	else if(0 == foundIndex)
	{
		foundIndex = NumStringsRemembered-1;
	}
	else
	{
		--foundIndex;
	}
	console.inputRenderString.text = console.rememberedStrings[foundIndex];

}

void InputConsole::Initialize()
{
	//EDEventManager::EventManager::GetInstance()->RegisterForEvent("ToggleConsole", 0, 
	//	ToggleConsoleCallback);
	//EDEventManager::EventManager::GetInstance()->RegisterForEvent("CompleteConsoleString", 0, 
	//	CompleteConsoleStringCallback);
	//EDEventManager::EventManager::GetInstance()->RegisterForEvent("RememberConsoleString", 0, 
	//	RememberConsoleStringCallback);

	inputRenderString.text = L"";
	completionRenderString.text = L"";
	float heightDiv = 32.0f;
	//backgroundSprite.LoadFromFile("GDModel/mdl_InputConsole.xml", (float)WorldData::GetScreenWidth()/2,
	//	(float)WorldData::GetScreenHeight() - (float)WorldData::GetScreenHeight()/heightDiv, 
	//	(float)WorldData::GetScreenWidth(), (float)WorldData::GetScreenHeight()/(heightDiv/2));
	RECT consoleRect;
	consoleRect.top = (Renderer::GetResolutionHeight() / 32) * 31;
	consoleRect.bottom = Renderer::GetResolutionHeight();
	consoleRect.left = 0;
	consoleRect.right = Renderer::GetResolutionWidth();

	ContentHandle<RenderTexture2D> spriteTexture = 
		ContentManager::Load<RenderTexture2D>("Textures/ConsoleBackground.dds");
	backgroundSprite.Initialize(spriteTexture.GetContent()->GetShaderResourceView(), consoleRect);
}

void InputConsole::UpdateString(RenderString &renderString, wstring &textString)
{
	renderString.text = textString;
	renderString.position = XMFLOAT3(
		static_cast<float>(xRenderString), 
		static_cast<float>(yRenderString), 
		0);
}

string wstringToString(wstring input)
{
	int len;
	int slength = (int)input.length();
	len = WideCharToMultiByte(CP_ACP, 0, input.c_str(), slength, 0, 0, 0, 0); 
	std::string ret(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, input.c_str(), slength, &ret[0], len, 0, 0); 
	return ret;
}

void InputConsole::ProcessKey(char key)
{
	// Don't do anything when disabled and don't process the ~
	if(!isEnabled || '`' == key || VK_TAB == key || VK_ESCAPE == key)
		return;

	if(VK_RETURN == key)
	{
        std::wstring dataString = L"";
        std::wstring::size_type index = inputRenderString.text.find_first_of(L" ");
        if((std::wstring::size_type) -1 != index)
        {
            dataString = inputRenderString.text.substr(index+1, inputRenderString.text.length());
            inputRenderString.text[index] = 0;
        }
		EDEventManager::EventManager *em = EDEventManager::EventManager::GetInstance();

		string temp = wstringToString(inputRenderString.text);

		em->PostEvent(temp.c_str(), (void *)dataString.c_str(), 
			EDEventManager::EM_PRIORITY_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);

		RememberInputString();

        inputRenderString.text = L"";
        completionRenderString.text = L"";

	}
	else if(VK_BACK == key)
	{
		if(inputRenderString.text != L"")
		{
			inputRenderString.text.erase(inputRenderString.text.end()-1);
		}
	}
	else
	{
		inputRenderString.text += key;
		completionRenderString.text = L"";		
		set<const wstring>::iterator iter = registeredCommands.begin();
		set<const wstring>::iterator iterEnd = registeredCommands.end();
		for(; iter != iterEnd; ++iter)
		{
			if(0 == (*iter).find(inputRenderString.text))
			{
				completionRenderString.text = (*iter);
				break;
			}
		}
	}

	UpdateString(inputRenderString, inputRenderString.text);
	UpdateString(completionRenderString, completionRenderString.text);
}

void InputConsole::Render()
{
	backgroundSprite.SetShouldRender(isEnabled);
	if(!isEnabled)
		return;

	RECT consoleRect;
	consoleRect.top = (Renderer::GetResolutionHeight() / 32) * 31;
	consoleRect.bottom = Renderer::GetResolutionHeight();
	consoleRect.left = 0;
	consoleRect.right = Renderer::GetResolutionWidth();

	yRenderString = consoleRect.top;

	backgroundSprite.SetDestinationRect(consoleRect);
	completionRenderString.QueueRender();
	inputRenderString.QueueRender();
}

void InputConsole::TypingCallback(char typed)
{
	InputConsole &console = InputConsole::GetReference();

	if(console.GetEnabled())
	{
		console.ProcessKey(typed);
	}
}

unsigned int InputConsole::RegisterCommand(const wstring &eventName, void *listener, 
	EDEventManager::EM_CALLBACK callback)
{
	RegisterExistingCommand(eventName);
	string temp = wstringToString(eventName);
	return EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		temp, listener, callback);
}

void InputConsole::RegisterExistingCommand(const wstring &eventName)
{
	registeredCommands.insert(eventName);
}

void InputConsole::RememberInputString()
{
	++lastRememberedIndex;
	if(lastRememberedIndex >= NumStringsRemembered)
		lastRememberedIndex = 0;
	rememberedStrings[lastRememberedIndex] = inputRenderString.text;
}
