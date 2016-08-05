
#include "CustomAssert.h"


namespace EDUtilities
{
	int EDAssertBackend(const char *message, const char *expression, const char *function, const char *line, const char *file)
	{
		const char *messageFormat = "%s\n\nModule: %s\nFailed in: %s:%s\nFunction: %s\nExpression: %s\n\nTo ignore this assertion once click 'Continue'.\nTo disable this assertion click 'Try Again'.\nTo break in the debugger press 'Cancel'";
		static size_t formatLength = strlen(messageFormat);

		char moduleName[MAX_PATH+1]= {'\0'};
		if(GetModuleFileNameA(NULL, moduleName, MAX_PATH) == 0)
		{
			sprintf_s(moduleName, "<program name unknown>");
		}

		size_t buffSizeInBytes = formatLength + strlen(message) + strlen(expression) + strlen(function) + strlen(line) + strlen(file) + strlen(moduleName);
		char *buff = (char *)malloc(buffSizeInBytes);

		// Give us something to double click in the output window
		sprintf_s(buff, buffSizeInBytes, "%s(%s): Assertion failed! %s\n", file, line, expression);
		OutputDebugStringA(buff);

		fprintf(stderr, "Assertion failed: %s, file %s, line %s\n", expression, file, line);

		sprintf_s(buff, buffSizeInBytes, messageFormat, message, moduleName, file, line, function, expression);
		int assertResponse = MessageBoxA(0, buff, "Assertion failed", MB_ICONERROR | MB_CANCELTRYCONTINUE);

		// TODO: Stuff 
		free(buff);

		return assertResponse;
	}  
}
