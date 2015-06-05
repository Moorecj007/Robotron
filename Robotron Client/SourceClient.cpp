/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : SourceClient.cpp
* Description : Source file to begin Robotron Server
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma comment(lib, "Winmm.lib")

// Library Includes
#include <winsock2.h>
//#include <windows.h>		// Include all the windows headers.
#include <windowsx.h>		// Include useful macros.

// Local Includes
#include "GameClient.h"

// Defines and Macros
#define WIN32_LEAN_AND_MEAN
#define WINDOW_CLASS_NAME L"ROBOTRON"


#ifdef _DEBUG
	// Visual Leak Detector to be run only if in DEBUG mode
	//#include "vld.h"
	//#define D3D_DEBUG_INFO
#endif // _DEBUG

// Prototypes
LRESULT CALLBACK WindowProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);

/***********************
* WindowProc: Process the window
* @author: Callan Moore
* @Parameter: _hWnd: Handle to the Window sending the message
* @Parameter: _uiMsg: The message ID being sent
* @Parameter: _wParam: Additional detail about the message being sent
* @Parameter: _lParam: Additional detail about the message being sent
* @return: LRESULT: The resulting value
********************/
LRESULT CALLBACK WindowProc(HWND _hWnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam)
{
	PAINTSTRUCT paintStruct;		// Used in WM_PAINT.
	HDC hDC;						// Handle to a device context.

	// Switch case dependent on the message sent
	switch (_uiMsg)
	{
		case WM_CREATE:
		{	
			return (0);
		}
		break;
		case WM_PAINT:
		{
			hDC = BeginPaint(_hWnd, &paintStruct);	// Prepares the window for painting
			EndPaint(_hWnd, &paintStruct);			// Marks the ending of the window being painted

			return (0);
		}
		break;
		case WM_DESTROY:
		{
			// Kill the application, this sends a WM_QUIT message.
			PostQuitMessage(0);
			return (0);
		}
		break;
		case WM_MOUSEMOVE:
		{
			// Get the position of the mouse when it moves
			int iXpos = GET_X_LPARAM(_lParam);
			int iYpos = GET_Y_LPARAM(_lParam);
			CGameClient::GetInstance().SetMousePos(iXpos, iYpos);	
		}
		break;
		case WM_LBUTTONDOWN:
		{
			// Tells the game the Left Mouse Button is currently down
			CGameClient::GetInstance().SetLeftMouseClick(true);
		}
		break;
		case WM_LBUTTONUP:
		{
			// Tells the game the Left Mouse Button is no longer down
			CGameClient::GetInstance().SetLeftMouseClick(false);
			CGameClient::GetInstance().ChangeMenuSelection();
		}
		break;
		default: break;
	} // End switch.

	// Process any messages left to process
	return (DefWindowProc(_hWnd, _uiMsg, _wParam, _lParam));
}

/***********************
* WinMain: Initialises the program to start and creates the window
* @author: Callan Moore
* @Parameter: _hInstance: Instance handle that Windows generates for your application
* @Parameter: _hPrevInstance: Tracker for the previous instance for the application
* @Parameter: _lpCmdline: Wide char string that contains the passed in arguments
* @Parameter: _iCmdshow: Integer passed in during launch, indicating how the application window is opened
* @return: int: Program ends here
********************/
int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nCmdShow)
{
	WNDCLASSEX winClass;	// This will hold the class we create.
	HWND hWnd;				// Generic window handle.
	MSG uiMsg;				// Generic message.

	const int kiScreenWidth = 1000;
	const int kiScreenHeight = 1000;

	// Fills in the window class structure.
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WindowProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = _hInstance;
	winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = WINDOW_CLASS_NAME;
	winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Registers the window class
	if (!RegisterClassEx(&winClass))
	{
		return (0);
	}

	hWnd = CreateWindowEx(	NULL,								// Extended style.
							WINDOW_CLASS_NAME,					// Class.
							L"Robotron Client",						// Title.
							WS_VISIBLE | WS_CAPTION | WS_BORDER | WS_SYSMENU,// Windowed Mode
							0, 0,								// Initial x,y position for the top left corner of the window
							kiScreenWidth, kiScreenHeight,		// Initial width, height of the window
							NULL,								// Handle to parent.
							NULL,								// Handle to menu.
							_hInstance,							// Instance of this application.
							NULL);								// Extra creation parameters.

	// Check the window was created successfully.
	if (!hWnd)
	{
		return (0);
	}

	// Create the Game Object
	CGameClient& rGameInstance = CGameClient::GetInstance();
	rGameInstance.Initialise(hWnd, kiScreenWidth, kiScreenHeight);

	// Enter main event loop.
	while (true)
	{
		while (PeekMessage(&uiMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&uiMsg);
			DispatchMessage(&uiMsg);
		}

		if (uiMsg.message == WM_QUIT)
		{
			break;
		}

		rGameInstance.RenderOneFrame();
	}

	// Delete the Game Instance
	rGameInstance.DestroyInstance();

	// Return to Windows
	return (static_cast<int>(uiMsg.wParam));
}