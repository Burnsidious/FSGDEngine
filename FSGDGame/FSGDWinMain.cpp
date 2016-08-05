// EDGame.cpp : Defines the entry point for the application.
//


#include <shellapi.h> // CommandLineToArgvW
#include <windowsx.h>

#include "FSGDWinMain.h"
#include "GameBase.h"
#include "FSGDGame.h"
#include "WorldData.h"
#include "./InputConsole.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDEventManager/EventManager.h"
#include "../EDInput/WindowsInput.h"
#include "../EDUtilities/InternalOutput.h"
#include "../EDMemoryManager/MemoryManager.h"

#include "./WndProcDebug.h"
using namespace EDUtilities;
using EDRendererD3D::Renderer;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND globalHWnd;
GameBase *globalGamePtr;
HINSTANCE   app;
HWND        wnd;
bool globalLockAspect = true;
float globalAspectRatio = 9.0f/16.0f;

#ifdef _WIN64
// Raw input data
#define RIBSize  48
#else
#define RIBSize  40
#endif

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
const std::map<std::string, std::string> GetCommandLineArgs();

int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    MyRegisterClass(hInstance);

    // Initialize instances from a single threaded environment since these GetInstance() methods are
	//   not thread safe.
	EDMemoryManager::MemoryManager *memoryManagerPointer = EDMemoryManager::MemoryManager::GetInstance();
    EDEventManager::EventManager *eventManagerPointer = EDEventManager::EventManager::GetInstance();


	globalGamePtr = new Game;

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDGAME));

    // Main message loop:
    ZeroMemory( &msg, sizeof( msg ) );
    while( msg.message != WM_QUIT )
    {
        while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

		EDInput::Update(0.0f);
        globalGamePtr->Update();
        globalGamePtr->Render();
    }

	delete globalGamePtr;

    return (int) msg.wParam;
}

/// Parses the command line string and stores all arguments in a key:value map of
/// strings. All arguments must begin with a single dash character '-' and are case
/// sensitive. Keys and values are split between a single equals sign character '='.
/// Arguments without a value are mapped to an empty string for their value.
/// \return Key:Value map of command line arguments.
const std::map<std::string, std::string> GetCommandLineArgs()
{
	int argc = 0;
	LPWSTR *argv = NULL;
	std::map<std::string, std::string> argMap;

	// Easiest way to quickly convert the single string command line into argv/argc parts
	argv = CommandLineToArgvW(GetCommandLine(), &argc);
	if(argv == NULL)
	{
		printf("CommandLineToArgvW failed with error %d!", GetLastError());
		return argMap;
	}

	for(int argIndex = 0; argIndex < argc; ++argIndex)
	{
		std::wstring currentArg = argv[argIndex];

		// Only accepting arguments beginning with '-'
		if(currentArg.length() == 0 || currentArg[0] != '-')
		{
			continue;
		}

		std::wstring argNameW;
		std::wstring argValueW;

		// Key:Value is split between the '=' character. If no character exists then only use the Key
		//    and set the Value to the default string.
		int splitAt = static_cast<int>(currentArg.find('='));
		if(splitAt != -1)
		{
			// Name is everything between the '-' and '=' characters
			argNameW = currentArg.substr(1, splitAt-1);
			// Value is everything after the '=' character
			argValueW = currentArg.substr(splitAt+1, (currentArg.length()-splitAt));
		}
		else
		{
			// Name is everything after the '-' character
			argNameW = currentArg.substr(1, currentArg.length()-1);
		}

		if(argNameW.length() == 0)
		{
			continue;
		}

		// Not sure if we're supporting unicode arguments or not, so it's just converting over to
		//  standard single byte character strings since they're generally easier to deal with.
		std::string argName = std::string(argNameW.begin(), argNameW.end());
		std::string argValue = std::string(argValueW.begin(), argValueW.end());
		argMap[argName] = argValue;
	}

	LocalFree(argv);
	return argMap;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    ZeroMemory( &wcex, sizeof( wcex ) );
    wcex.cbSize         = sizeof( WNDCLASSEX );             // size of window structure
    wcex.lpfnWndProc    = WndProc;						// message callback
    wcex.lpszClassName  = L"Title";             // class name
	wcex.hInstance      = hInstance;		                // handle to application
    wcex.hCursor        = LoadCursor( NULL, IDC_ARROW );    // default cursor
    wcex.hbrBackground  = ( HBRUSH )( COLOR_WINDOWFRAME );  // background brush
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDGAME));
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EDGAME);
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

	// WM_SIZE message will not fire on alt+enter without the "+20", unsure why at this time
	RECT window_size = { 0, 0, WorldData::GetScreenWidth(), WorldData::GetScreenHeight()};

	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, true);

    globalHWnd = CreateWindow(	L"Title", 
								L"FSGD Engine", 
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT, 
								CW_USEDEFAULT, 
								window_size.right - window_size.left, 
								window_size.bottom - window_size.top,
								NULL, 
								NULL, 
								hInstance, 
								NULL);

    if (!globalHWnd)
    {
        return FALSE;
    }


	EDInput::Initialize(globalHWnd);
	EDInput::SetTypingCallback(InputConsole::TypingCallback);

	globalGamePtr->Initialize();

	ShowWindow(globalHWnd, nCmdShow);

    UpdateWindow(globalHWnd);
    return TRUE;
}

void SetResolution(UINT _width, UINT _height)
{
	globalAspectRatio = (float)_height/(float)_width; 
	WorldData::SetScreenWidth(_width);
	WorldData::SetScreenHeight(_height);

	Renderer::SetResolution(_width, _height);
	RenderController::GetInstance()->UpdateResolution();

	SetWindowPos(globalHWnd, HWND_TOPMOST, 0, 0, _width, _height, SWP_NOMOVE);
}

LRESULT OnSizing( WPARAM wParam, LPARAM lParam)
{
	RECT sz;
    int w, h;
 
    // If the aspect is locked, resize the window equally
    if (globalLockAspect)
    {
        // Copy over the new size that was determined by windows
        memcpy(&sz, (const void *) lParam, sizeof(RECT));
 
        // Calculate the width and height of the window
        w = sz.right - sz.left;
        h = sz.bottom - sz.top;
 
        switch (wParam)
        {
            case WMSZ_LEFT:
            case WMSZ_RIGHT:
                // Modify the Heigh of the window
                sz.bottom = LONG(w * globalAspectRatio) + sz.top;
                break;
 
            case WMSZ_TOP:
            case WMSZ_BOTTOM:
                // Modify the Width of the window
                sz.right = LONG(h * (1 / globalAspectRatio)) + sz.left;
                break;
 
            case WMSZ_TOPRIGHT:
            case WMSZ_TOPLEFT:
            case WMSZ_BOTTOMRIGHT:
            case WMSZ_BOTTOMLEFT:
                // Adjust the width and height of the window to match aspect ratio
                if (float(h) / float(w) > globalAspectRatio)
                {
                    w = int(float(h) / globalAspectRatio);
                }else{
                    h = int(float(w) * globalAspectRatio);
                }
 
                // Adjust Height
                if (wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT)
                {
                    sz.top = sz.bottom - h;
                }else{
                    sz.bottom = sz.top + h;
                }
 
                // Adjust Width
                if (wParam == WMSZ_TOPLEFT || wParam == WMSZ_BOTTOMLEFT)
                {
                    sz.left = sz.right - w;
                }else{
                    sz.right = sz.left + w;
                }
                break;
        }
 
        // Copy back the size of the window
        memcpy((void *) lParam, &sz, sizeof(RECT));
 
        return TRUE;
    }
 
    return FALSE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND			- process the application menu
//  WM_PAINT			- Paint the main window
//  WM_DESTROY			- post a quit message and return
//  WM_CHAR				- posts typed letters, both upper and lower case
//  WM_KEYUP			- posts when a key comes up, only upper case
//  WM_KEYDOWN			- posts when a key comes up, only upper case
//  WM_LBUTTONDOWN		- left mouse button down
//	WM_LBUTTONDBLCLK	- left mouse button dobule clicked
//  WM_LBUTTONUP		- left mouse button came up
//  WM_RBUTTONDOWN		- right mouse button down
//	WM_RBUTTONDBLCLK	- right mouse button dobule clicked
//  WM_RBUTTONUP		- right mouse button came up
//  WM_MBUTTONDOWN		- middle mouse button down
//	WM_MBUTTONDBLCLK	- middle mouse button dobule clicked
//  WM_MBUTTONUP		- middle mouse button came up
//  WM_XBUTTONDOWN		- browser button down check.  flag to see what one it was
//	WM_XBUTTONDBLCLK	- browser button dobule clicked.  flag to see what one it was
//  WM_XBUTTONUP		- browser button came up.  flag to see what one it was
//  WM_MOUSELEAVE		- special message sent when mousevents are captured
//  WM_NCACTIVATE		- sent when a user clicked outside the window
//  WM_MOUSEMOVE		- the mouse has moved this update
//  WM_DEVICECHANGE		- check for new devices
//  WM_SYSKEYDOWN		- sent when special keys are pressed
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    static HGLRC hRC = NULL;
	
#ifdef _OUTPUT_WINDOWS_MESSAGES_
	//if you want to filter all unhandled input messages keep the if check in
	if (0 != EDInput::InputProc(hWnd, message, wParam, lParam) )
	{
		switch(message)
		{
			//Filter messages that should not be seen in the output window
		case WM_SETCURSOR:
		case WM_NCHITTEST:
			break;
		default:
			{
				CheckMessage(message);
			}
		};
		
	}
#else
	EDInput::InputProc(hWnd, message, wParam, lParam);
#endif

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
		case ID_OPTIONS_LOCKASPECTRATIO:
			{
				HMENU theMenu = GetMenu(globalHWnd);
				UINT checked = GetMenuState(theMenu, ID_OPTIONS_LOCKASPECTRATIO, MF_BYCOMMAND);
				CheckMenuItem(theMenu, ID_OPTIONS_LOCKASPECTRATIO, checked ? MF_UNCHECKED : MF_CHECKED );
				globalLockAspect = !checked;
			}
			break;
		case ID_SETRESOLUTION_1920X1080:
			SetResolution(1920, 1080);
			break;
		case ID_SETRESOLUTION_1600X900:
			SetResolution(1600, 900);
			break;
		case ID_SETRESOLUTION_1280X720:
			SetResolution(1280, 720);
			break;
		case ID_SETRESOLUTION_800X600:
			SetResolution(800, 600);
			break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_SIZE:
		//assert(0);
		globalGamePtr->Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_SIZING:
		OnSizing( wParam, lParam);
		break;
	// Toggle full screen upon alt-enter 
	case WM_SYSKEYDOWN:
        {
			switch( wParam )
            {
                case VK_RETURN:
                {
                    if((lParam & (1 << 29) ) != 0)
                    {
						//OutputDebugStringA("Alt Enter\n");
						SetResolution(WorldData::GetScreenWidth(), WorldData::GetScreenHeight() );
                    }
                }
				break;
            }
        }
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

