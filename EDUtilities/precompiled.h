#pragma once



#include "Platform.h"

#if defined(_DEBUG)
#define AUTO_ENABLE_LEAKDETECTION
#endif
//#define USE_WER

#if defined(PLATFORM_WINDOWS)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#if defined(_UNICODE)
#define DBGHELP_TRANSLATE_TCHAR
#endif


#include <Windows.h>
//#include <shellapi.h> // CommandLineToArgvW
//#include <windowsx.h>
#include <tchar.h>
#include <dbgHelp.h>
#if defined(USE_WER)
#include <WERAPI.H>
#pragma comment(lib,"wer.lib")
#endif

#include <D3D11.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <directxmath.h>
//using namespace DirectX;


#else

#include <malloc.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <execinfo.h>

#include "CoreDumper.h"

#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <atlbase.h> 
#include <stdarg.h>

#include <locale>
#include <memory>
#include <string>
#include <sstream>

#include <algorithm>
#include <limits>
#include <functional>

#include <queue>
#include <vector>
#include <list>
#include <unordered_map>
#include <map>
#include <set>
#include <iomanip>

#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

#include <iostream>
#include <fstream>
#include "Macros.h"

