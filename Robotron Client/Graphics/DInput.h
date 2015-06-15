/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : DInput.h
* Description : Handles all input from external devices
* Author :	Callan Moore
* Mail :	Callan.Moore@mediadesign.school.nz
*/

#pragma once

#ifndef __DINPUT_H__
#define __DINPUT_H__

// Library Includes
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>

// Local Includes
#include "../../Common Files/Generic_Defines.h"

class CDInput
{
public:
	// Constructors / Destructors

	/***********************
	* CDInput: Default Constructosr for DInput class
	* @author: Callan Moore
	********************/
	CDInput();

	/***********************
	* ~CDInput: Default Destructor for DInput class
	* @author: Callan Moore
	********************/
	~CDInput();

	// Getters

	/***********************
	* GetControls: Retrieve the current state of the control input
	* @author: Callan Moore
	* @return: Controls: structure containing the current state of the controls
	********************/
	Controls GetControls();

	/***********************
	* GetMousePos: Retrieve the current Mouse Position
	* @author: Callan Moore
	* @return: POINT: THe mouse position as a point
	********************/
	POINT GetMousePos();

	// Setters

	// Prototypes

	/***********************
	* Initialise: Initialise the DInput for use
	* @author: Callan Moore
	* @parameter: _hInstance: The instance of the application window
	* @parameter: _hWnd: The handle to the application window
	* @parameter: _iScreenWidth: The screen width of the application
	* @parameter: _iScreenHeight: The screen height of the application
	* @return: bool: Successful Initialization (or not)
	********************/
	bool Initialise(HINSTANCE _hInstance, HWND _hWnd, int _iScreenWidth, int _iScreenHeight);

	/***********************
	* InitialiseKeyboard: Initialise the Keyboard
	* @author: Callan Moore
	* @parameter: _hWnd: The handle to the application window
	* @return: bool: Successful Initialization (or not)
	********************/
	bool InitialiseKeyboard(HWND _hWnd);

	/***********************
	* InitialiseMouse: Initialise the Mouse
	* @author: Callan Moore
	* @parameter: _hWnd: The handle to the application window
	* @return: bool: Successful Initialization (or not)
	********************/
	bool InitialiseMouse(HWND _hWnd);

	/***********************
	* Shutdown: Shutdown the DirectIput class
	* @author: Callan Moore
	* @return: void
	********************/
	void Shutdown();

	/***********************
	* Process: Process the Inputs
	* @author: Callan Moore
	* @return: void
	********************/
	void Process();

	/***********************
	* Shutdown: Shutdown the DirectIput class
	* @author: Callan Moore
	* @parameter: _pDIDevice: Pointer to the Input device to read
	* @parameter: _pDataBuffer: Pointer to a buffer to store the read data
	* @parameter: _iBufferSize: Size of the buffer to store the read data
	* @return: void
	********************/
	bool ReadDevice(IDirectInputDevice8* _pDIDevice, void* _pDataBuffer, int _iBufferSize);

	/***********************
	* ProcessInput: Process the inputs from the devices
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessInput();

	/***********************
	* ProcessKeyboard: Process the input from the Keyboard
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessKeyboard();

	/***********************
	* ProcessMouse: Process the input from the Mouse
	* @author: Callan Moore
	* @return: void
	********************/
	void ProcessMouse();

private:
	// Window Variables
	HWND m_hWnd;
	int m_iScreenWidth;
	int m_iScreenHeight;
	

	// Game Variables
	bool m_bControllerInput;
	Controls m_activatedControls;

	// Input Variables
	IDirectInput8* m_pDirectInput;
	IDirectInputDevice8* m_pKeyboard;
	IDirectInputDevice8* m_pMouse;

	// Mouse Variables
	POINT m_ptMousePos;
	DIMOUSESTATE m_mouseState;

	// Keyboard Variables
	unsigned char m_keyboardState[256];

	
};

#endif // __DINPUT_H__

