#pragma once



#include "../EDRendererD3D/RenderString.h"
#include "../EDRendererD3D/RenderSprite.h"
#include "../EDEventManager/EventManager.h"

/// This singleton is used receive text input used to fire events.
/// The input received should be the name of an event to fire.  
/// Any data received after a space is passed as data to the event.
class InputConsole
{
public:
	~InputConsole(void);

    /// Gets a reference to instance of the InputConsole.
	static InputConsole &GetReference();
    /// Deletes the instance to the InputConsole.
	static void DeleteInstance();

    /// This method is used to process input received.
    /// \param key - The VK code of the key that should be proccessed.  
    /// This is typically mapped to user input, like the keyboard.
	void ProcessKey(char key);
    /// Renders the string data of the input that has been received.
	void Render();

	inline void SetEnabled(bool enabled) { isEnabled = enabled; }
	inline bool GetEnabled() { return isEnabled; }
	static void TypingCallback(char typed);

	/// Registers an event to be triggered when command is received.
	/// \param eventName - Name of the event to register for as well as the command to trigger it
	/// \param listener - Object registering as a listener for this event.
	/// \param callback - Function to call whenever event is raised.
	/// \return Event Id of the eventName.
	unsigned int RegisterCommand(const std::wstring &eventName, void *listener, 
		EDEventManager::EM_CALLBACK callback);
	/// Adds a command to th console auto-complete list.  This command should already have an 
	/// event associated with it, otherwise use the RegisterCommand method.
	/// \param eventName - Name of the event that has already been added to the event manager
	void RegisterExistingCommand(const std::wstring &eventName);

	    /// An event used to toggle the console's enabled state.
	static void ToggleConsoleCallback(unsigned int eventId, const void *eventData, void *listener);

	/// An event used to attempt to auto-complete the input string.
	static void CompleteConsoleStringCallback(unsigned int eventId, const void *eventData, void *listener);

	/// An event used to set the input string to a remembered input string
	static void RememberConsoleStringCallback(unsigned int eventId, const void *eventData, void *listener);

	inline const std::set<const std::wstring>& GetCommands() { return registeredCommands; }

private:
	// Singleton fun
	InputConsole(void);
	InputConsole(const InputConsole&) {}
	InputConsole &operator=(const InputConsole &) {}

	static InputConsole *instancePtr;

    /// Whether the console is currently open and receiving input.
	bool isEnabled;

	/// The maximum length of characters to be rendered by the RenderString instances.
	static const int MaxStringLength = 64;
	/// The RenderString used to render user inputed strings
	EDRendererD3D::RenderString inputRenderString;
	/// The RenderString used to render suggested possible completion strings
	EDRendererD3D::RenderString completionRenderString;

	EDRendererD3D::RenderSprite backgroundSprite;

	/// The pixel space loaction on screen for RenderStrings
	UINT xRenderString;
	UINT yRenderString;

	/// The background GUI element
	//EDRendererD3D::RenderSprite backgroundSprite;

	std::set<const std::wstring> registeredCommands;

	/// The number of input strings to remember for inputing again
	const static int NumStringsRemembered = 5;
	/// The index to the last string remembered
	int lastRememberedIndex;
	/// The input strings to remember for inputing again
	std::wstring rememberedStrings[NumStringsRemembered];

	/// Helper method for updating the remembered input strings
	void RememberInputString();

    /// Initializes the console for first use.  This is called when the instance is created.
	void Initialize();

	/// Helper method for making a RenderString instance match the text of a string class instance
	/// as well as control where on the screen the text will be printed.
	void UpdateString(EDRendererD3D::RenderString &renderString, std::wstring &textString);


};
