#pragma once

namespace EDUtilities
{
	int EDAssertBackend(const char *message, const char *expression, const char *function, const char *line, const char *file);

	// Just a way of getting the preprocessor to use string "__LINE__" instead of the integer value __LINE__
	#define TOSTRING(value) #value
	#define EVALTOSTRING(expression) TOSTRING(expression)

	#ifdef _DEBUG
		// NOTE: ( (void)0, <expression> ) is just a way to suppress warnings (c4127) from using constant conditional expressions,
		//   which lets us be able to have unconditional asserts such as Assert(false, "blah"); 
		#define ASSERT(expression, message) \
		do { \
			static bool ignoreAssertion_ = false; \
			if(!ignoreAssertion_ && !((void)0, (expression))) \
			{ \
				int assertResponse_ = EDUtilities::EDAssertBackend(message, #expression, __FUNCTION__, EVALTOSTRING(__LINE__), __FILE__); \
				if(assertResponse_ == IDCANCEL) \
				{ \
					DebugBreak(); \
				} \
				else if(assertResponse_ == IDTRYAGAIN) \
				{ \
					ignoreAssertion_ = true; \
				} \
			} \
		} while((void)0, 0)
	#else
		#define ASSERT(expression, message) (void)0
	#endif
}