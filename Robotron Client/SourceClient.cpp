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

// Defines and Macros
#define WIN32_LEAN_AND_MEAN
#define WINDOW_CLASS_NAME L"ROBOTRON"
#define _WINSOCKAPI_

// Library Includes
#include <winsock2.h>
//#include <windows.h>		// Include all the windows headers.
#include <windowsx.h>		// Include useful macros.

// Local Includes
#include "Hub_Client.h"
#include "resource.h"

#ifdef _DEBUG
	// Visual Leak Detector to be run only if in DEBUG mode
	#include "vld.h"
	#define D3D_DEBUG_INFO
#endif // _DEBUG

// Prototypes
LRESULT CALLBACK WindowProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);

// Global Variables
HCURSOR g_hCursor;

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
		case WM_SETCURSOR:
		{
			SetCursor(g_hCursor);
			// Return Success.
			return (0);
		}
		case WM_DESTROY:
		{
			// Kill the application, this sends a WM_QUIT message.
			PostQuitMessage(0);
			return (0);
		}
		break;
		case WM_KEYDOWN:
		{
			if (CHub_Client::GetInstance().GetTextInput() == true)
			{
				if (_wParam == 16)	// Shift Key
				{
					CHub_Client::GetInstance().m_pbKeyDown[_wParam] = true;
				}
				if (((_lParam >> 30) & 1) != 1)
				{
					CHub_Client::GetInstance().ProcessTextInput(_wParam);
				}
			}
			//else
			//{
			//	CHub_Client::GetInstance().m_pbKeyDown[_wParam] = true;
			//}
		}
		break;
		case WM_KEYUP:
		{
			CHub_Client::GetInstance().m_pbKeyDown[_wParam] = false;
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

	int iScreenWidth = 1000;
	int iScreenHeight = 1000;

	g_hCursor = LoadCursor(_hInstance, MAKEINTRESOURCE(IDC_CROSSHAIR));

	// Fills in the window class structure.
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WindowProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = _hInstance;
	winClass.hIcon = NULL;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = WINDOW_CLASS_NAME;
	winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_CROSSHAIR)));

	// Registers the window class
	if (!RegisterClassEx(&winClass))
	{
		return (0);
	}

	RECT adjustRect = { 0, 0, iScreenWidth, iScreenHeight };
	AdjustWindowRect(&adjustRect, WS_VISIBLE | WS_CAPTION | WS_BORDER | WS_SYSMENU, false);
	iScreenHeight = adjustRect.bottom - adjustRect.top;
	iScreenWidth = adjustRect.right - adjustRect.left;

	hWnd = CreateWindowEx(	NULL,								// Extended style.
							WINDOW_CLASS_NAME,					// Class.
							L"Robotron Client",						// Title.
							WS_VISIBLE | WS_CAPTION | WS_BORDER | WS_SYSMENU,// Windowed Mode
							0, 0,								// Initial x,y position for the top left corner of the window
							iScreenWidth, iScreenHeight,		// Initial width, height of the window
							NULL,								// Handle to parent.
							NULL,								// Handle to menu.
							_hInstance,							// Instance of this application.
							NULL);								// Extra creation parameters.

	// Check the window was created successfully.
	if (!hWnd)
	{
		return (0);
	}

	POINT point{ (iScreenWidth / 2), (iScreenHeight / 2) };
	ClientToScreen(hWnd, &point);
	SetCursorPos(point.x, point.y);

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// Create the Game Object
	CHub_Client& rApplicationInstance = CHub_Client::GetInstance();
	rApplicationInstance.Initialise(_hInstance, hWnd, clientRect.right, clientRect.bottom);
	bool bOnline = true;

	// Enter main event loop.
	while (bOnline)
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

		bOnline = rApplicationInstance.RenderOneFrame();
	}

	// Delete the Game Instance
	rApplicationInstance.DestroyInstance();

	// Return to Windows
	return (static_cast<int>(uiMsg.wParam));
}