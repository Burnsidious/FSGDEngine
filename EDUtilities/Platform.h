#pragma once

#if (defined(_MANAGED) && defined(_M_CEE_SAFE)) || (defined(_MANAGED) && defined(USE_MANAGED_SAFE))
#define API_MANAGED_SAFE
#elif defined(_MANAGED) && defined(_M_CEE_PURE)
#define API_MANAGED_PURE
#elif defined(_MANAGED)
#define API_MANAGED_MIXED
#elif defined(_WIN32) || defined(_WIN64)
#define API_WINDOWS
#else
#define API_UNIX
#endif


#if defined(API_MANAGED_PURE) || defined(API_MANAGED_SAFE)
#define PLATFORM_MANAGED
#elif defined(API_WINDOWS) || defined(API_MANAGED_MIXED)
#define PLATFORM_WINDOWS
#define PLATFORM_NATIVE
#elif defined(API_UNIX)
#define PLATFORM_UNIX
#define PLATFORM_NATIVE
#endif