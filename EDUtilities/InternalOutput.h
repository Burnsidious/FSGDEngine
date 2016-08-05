#pragma once
#include "precompiled.h"

#define MAX_STRING_LENGTH 1024

namespace EDUtilities
{
    /// Manages an external console window.
    /// InternalOutput is a singleton class that can be used to spawn and write to an external console window.
    /// To use the singleton, get a reference then use the << operator to output data.
    /// Friend output methods can be added to, and for, any type.
    class InternalOutput
    {
    private:
        /// The handle to where the output should go.
        HANDLE outputHandle;

		std::mutex mutex;

		bool output_to_file;

		FILE* output_file;
		FILE* input_file;

        /// Private constructor to ensure singleton structure.
		InternalOutput();
		/// Private copy constructor to ensure singleton structure.
        InternalOutput(const InternalOutput&) {}
        /// Private assignment operator to ensure singleton structure.
        InternalOutput &operator=(const InternalOutput &) {}

        /// The one and only instance of this class.
		// to avoid double check locking I just made the instance static in getinstance
        //static InternalOutput *instancePtr;

        /// Creates the external console window and routes output to it.
        //void Create();

    public:
        ~InternalOutput(void);

        /// Returns a reference to the one instance of this class.
        static InternalOutput& GetReference();

        /// Deletes the one reference to this class.
        static void DeleteInstance();

		/// Output method for printing errors to the console.
		/// This method will format the string given to be in an error format.
        /// \param formatString - A format control string, ex. "X = %d".
        /// \sa sprintf
        /// \param ... - Optional arguments to be used in format control string.
		void Error(const char *formatString, ...);

        /// Output method used for C style strings.
        /// \param out - The output object, should be a reference to an InternalOutput.
        /// \param string - The null terminated character array to output.
        friend InternalOutput &operator<<(InternalOutput &out, const char *string);
		void LogMissingAsset(const char *string);

        /// Output method used for C style wide character strings.
        /// \param out - The output object, should be a reference to an InternalOutput.
        /// \param string - The null terminated wide character array to output.        
		friend InternalOutput &operator<<(InternalOutput &out, const WCHAR *string);
		void LogMissingAsset(const WCHAR *string);

		/// Output method used for class string.
        /// \param out - The output object, should be a reference to an InternalOutput.
        /// \param string - The string to output
        friend InternalOutput &operator<<(InternalOutput &out, const std::string &inString);

        /// Output method used for a character.
        /// \param out - The output object, should be a reference to an InternalOutput.
        /// \param letter - The character to output.
        friend InternalOutput &operator<<(InternalOutput &out, char letter);

        /// Output method used for a float.
        /// \param out - The output object, should be a reference to an InternalOutput.
        /// \param fVal - The float to output.
        friend InternalOutput &operator<<(InternalOutput &out, float fVal);

        /// Output method used for a double.
        /// \param out - The output object, should be a reference to an InternalOutput.
        /// \param dVal - The double to output.
        friend InternalOutput &operator<<(InternalOutput &out, double dVal);

        /// Output method used for a integer.
        /// \param out - The output object, should be a reference to an InternalOutput.
        /// \param iVal - The integer to output.
        friend InternalOutput &operator<<(InternalOutput &out, int iVal);

        /// Output method used for an unsigned integer.
        /// \param out - The output object, should be a reference to an InternalOutput.
        /// \param iVal - The unsigned integer to output.
        friend InternalOutput &operator<<(InternalOutput &out, UINT iVal);

        /// Output method used for a long.
        /// \param out - The output object, should be a reference to an InternalOutput.
        /// \param lVal - The long to output.
        friend InternalOutput &operator<<(InternalOutput &out, long lVal);
    };
}
