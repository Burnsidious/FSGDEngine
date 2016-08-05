#include "InternalOutput.h"
#include "Settings.h"

namespace EDUtilities
{
	InternalOutput::InternalOutput()
	{
		bool isFullscreen;
		EDUtilities::Settings::GetInstance()->GetSetting("Fullscreen", isFullscreen, false);
		EDUtilities::Settings::GetInstance()->GetSetting("OutputConsoleToFile", output_to_file, false);

		// Fullscreen rendering is broken by allocating this console
		if(!isFullscreen)
		{
			AllocConsole();
		}

		outputHandle = CreateFile(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

		// python isn't happy if stdout/in/err are not defined to point anywhere
		// redirect std out to a file, anything sent to internal output will also shoot to the file
		if (output_to_file == true)
		{
			freopen_s(&input_file, "CONIN$", "r", stdin); 
			freopen_s(&output_file, "../Logs/console_output.txt", "w", stdout); 
			freopen_s(&output_file, "../Logs/console_output.txt", "w", stderr); 
		}
		// in this case cout will be directed to our new console
		else
		{
			freopen_s(&input_file, "CONIN$", "r", stdin); 
			freopen_s(&output_file, "CONOUT$", "w", stdout); 
			freopen_s(&output_file, "CONOUT$", "w", stderr);
		}
	}

	InternalOutput::~InternalOutput(void) 
	{
		CloseHandle(outputHandle);

		if (input_file != nullptr)
		{
			fclose(input_file);
			input_file = nullptr;
		}

		if (output_file != nullptr)
		{
			fclose(output_file);		
			output_file = nullptr;
		}
	}
	
	InternalOutput& InternalOutput::GetReference()
	{
		static InternalOutput the_instance;
		return the_instance;
	}
	
	void InternalOutput::DeleteInstance()
	{
	}

	void InternalOutput::Error(const char *formatString, ...)
	{
		va_list ap;
		va_start(ap, formatString);

		char buffer[MAX_STRING_LENGTH];
		vsnprintf_s(buffer, MAX_STRING_LENGTH, MAX_STRING_LENGTH-1, formatString, ap);

		SetConsoleTextAttribute(outputHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
		(*this) << buffer;
		SetConsoleTextAttribute(outputHandle, FOREGROUND_RED | FOREGROUND_BLUE 
			| FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		va_end(ap);
//		assert(0);
	}

	InternalOutput &operator<<(InternalOutput &out, 
		const char *string)
	{
		WCHAR *buffer = new WCHAR[strlen(string)+1];

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string, 
			static_cast<int>(strlen(string)), buffer, static_cast<int>(strlen(string)));

		buffer[strlen(string)] = 0;

		out << buffer;

		delete [] buffer;
		return out;
	}

	void InternalOutput::LogMissingAsset(const char *string)
	{
		SetConsoleTextAttribute(outputHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		(*this) << string << L"\n";
		SetConsoleTextAttribute(outputHandle, FOREGROUND_RED | FOREGROUND_BLUE
			| FOREGROUND_GREEN);
	}

	InternalOutput &operator<<(InternalOutput &out, 
		const WCHAR *string)
	{
		// everything gets routed here eventually, not sure if writeconsole is thread safe and I was getting weird results so manually protect it
		char string_to_file[256];
		DWORD charWritten;
		out.mutex.lock();
			WriteConsole(out.outputHandle, string, static_cast<DWORD>(wcslen(string)), &charWritten, 0);

			if (out.output_to_file == true)
			{
				WideCharToMultiByte(CP_ACP, 0 , string, 256, string_to_file, 256, nullptr, nullptr);
				cout << string_to_file;
			}

		out.mutex.unlock();

		return out;
	}

	void InternalOutput::LogMissingAsset(const WCHAR *string)
	{
		SetConsoleTextAttribute(outputHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		(*this) << string << L"\n";
		SetConsoleTextAttribute(outputHandle, FOREGROUND_RED | FOREGROUND_BLUE
			| FOREGROUND_GREEN );
	}

	InternalOutput &operator<<(InternalOutput &out, 
		const std::string &inString)
	{
		return out << inString.c_str();
	}

	InternalOutput &operator<<(InternalOutput &out,
		char letter)
	{
		out << &letter;

		return out;
	}

	InternalOutput &operator<<(InternalOutput &out,
		float fVal)
	{
		const int BufferSize = MAX_STRING_LENGTH;
		WCHAR buffer[BufferSize];

		swprintf_s(buffer, BufferSize, L"%f", fVal);

		out << buffer;

		return out;
	}

	InternalOutput &operator<<(InternalOutput &out,
		double dVal)
	{
		const int BufferSize = MAX_STRING_LENGTH;
		WCHAR buffer[BufferSize];

		swprintf_s(buffer, BufferSize, L"%f", dVal);

		out << buffer;

		return out;
	}

	InternalOutput &operator<<(InternalOutput &out,
		int iVal)
	{
		const int BufferSize = MAX_STRING_LENGTH;
		WCHAR buffer[BufferSize];

		swprintf_s(buffer, BufferSize, L"%d", iVal);

		out << buffer;

		return out;
	}

	InternalOutput &operator<<(InternalOutput &out,
		UINT iVal)
	{
		const int BufferSize = MAX_STRING_LENGTH;
		WCHAR buffer[BufferSize];

		swprintf_s(buffer, BufferSize, L"%d", iVal);

		out << buffer;

		return out;
	}

	InternalOutput &operator<<(InternalOutput &out,
		long lVal)
	{
		const int BufferSize = MAX_STRING_LENGTH;
		WCHAR buffer[BufferSize];

		swprintf_s(buffer, BufferSize, L"%d", lVal);

		out << buffer;

		return out;
	}
}
