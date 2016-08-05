#pragma once

//Cleanup helper macros
#define SAFE_DELETE(p)	{ delete(p); (p) = nullptr; }
#define SAFE_ARRAYDELETE(p) { delete[](p); (p) = nullptr; }
#define SAFE_RELEASE(p) { if((p)) { (p)->Release(); (p) = nullptr; } }


//Text Constant macros
#define __WT(x)		L ## x

#define _WT(x)		__WT(x)
#define _AT(x)		x

#if !defined(_T)
#if defined(_UNICODE)
#define _T(x)		_WT(x)
#else
#define _T(x)		_AT(x)
#endif
#endif

#if defined(_UNICODE)
#define _TFILE		_FILEW_
#define _TFUNCTION	__FUNCTIONW__
#else
#define _TFILE		__FILE__
#define _TFUNCTION	__FUNCTION__
#endif

#if !defined(UNREFERENCED_PARAMETER)
#define UNREFERENCED_PARAMETER(P)			(P)
#endif

// concatenates the given stream elements in _expr and returns a const tchar* value (don't save this value for later use!!!)
#define s_cstr(expr_) (reinterpret_cast<tostringstream&>(reinterpret_cast<std::basic_ostream<tchar>&>(tostringstream()) << expr_)).str().c_str()
#define s_cstrA(expr_) (reinterpret_cast<std::ostringstream&>(reinterpret_cast<std::basic_ostream<char>&>(std::ostringstream()) << expr_)).str().c_str()
#define s_cstrW(expr_) (reinterpret_cast<std::wostringstream&>(reinterpret_cast<std::basic_ostream<wchar_t>&>(std::wostringstream()) << expr_)).str().c_str()

// concatenates the given stream elements in _expr and returns a tstring value
#define s_str(expr_) (reinterpret_cast<tostringstream&>(reinterpret_cast<std::basic_ostream<tchar>&>(tostringstream()) << expr_)).str()
#define s_strA(expr_) (reinterpret_cast<std::ostringstream&>(reinterpret_cast<std::basic_ostream<char>&>(std::ostringstream()) << expr_)).str()
#define s_strW(expr_) (reinterpret_cast<std::wostringstream&>(reinterpret_cast<std::basic_ostream<wchar_t>&>(std::wostringstream()) << expr_)).str()
